#include <iostream>
#include <fstream>
#include <string>
#include <limits> // For clearing the input buffer

using namespace std;

// Structure to store PPM image data
struct PPMImage {
    string magicNumber; // P6 for binary PPM
    int width;
    int height;
    int maxColorValue;
    unsigned char*** pixels; // 3D array for RGB pixels
};

// Function to skip comments in the PPM file
void skipComments(ifstream& file) {
    char ch;
    file >> ch;
    while (ch == '#') { // Skip comment lines
        file.ignore(numeric_limits<streamsize>::max(), '\n');
        file >> ch;
    }
    file.putback(ch); // Put the non-comment character back
}

// Function to allocate memory for the pixel data
unsigned char*** allocatePixels(int width, int height) {
    unsigned char*** pixels = new unsigned char**[height];
    for (int i = 0; i < height; ++i) {
        pixels[i] = new unsigned char*[width];
        for (int j = 0; j < width; ++j) {
            pixels[i][j] = new unsigned char[3]; // 3 channels for RGB
        }
    }
    return pixels;
}

// Function to free memory for the pixel data
void freePixels(unsigned char*** pixels, int width, int height) {
    for (int i = 0; i < height; ++i) {
        for (int j = 0; j < width; ++j) {
            delete[] pixels[i][j]; // Free each pixel's RGB channels
        }
        delete[] pixels[i]; // Free each row
    }
    delete[] pixels; // Free the entire pixel array
}

// Function to read a binary PPM file
bool readPPM(const string& filename, PPMImage& image) {
    ifstream file(filename, ios::binary);
    if (!file.is_open()) {
        cerr << "Error: Could not open file " << filename << endl;
        return false;
    }

    // Read the magic number
    file >> image.magicNumber;

    // Check if the file is in binary PPM format (P6)
    if (image.magicNumber != "P6") {
        cerr << "Error: File is not in binary PPM format (P6)" << endl;
        return false;
    }

    // Skip comments and read width, height, and max color value
    skipComments(file);
    file >> image.width >> image.height;
    skipComments(file);
    file >> image.maxColorValue;

    // Ignore the newline character after the header
    file.ignore(numeric_limits<streamsize>::max(), '\n');

    // Allocate memory for the pixel data
    image.pixels = allocatePixels(image.width, image.height);

    // Read the pixel data
    for (int i = 0; i < image.height; ++i) {
        for (int j = 0; j < image.width; ++j) {
            file.read((char*)image.pixels[i][j], 3); // Read R, G, B values
            if (file.fail()) {
                cerr << "Error: Failed to read pixel data at (" << i << ", " << j << ")" << endl;
                freePixels(image.pixels, image.width, image.height); // Free memory on error
                return false;
            }
        }
    }

    file.close();
    return true;
}

// Function to write a binary PPM file
bool writePPM(const string& filename, const PPMImage& image) {
    ofstream file(filename, ios::binary);
    if (!file.is_open()) {
        cerr << "Error: Could not create file " << filename << endl;
        return false;
    }

    // Write the header
    file << image.magicNumber << "\n";
    file << image.width << " " << image.height << "\n";
    file << image.maxColorValue << "\n";

    // Write the pixel data
    for (int i = 0; i < image.height; ++i) {
        for (int j = 0; j < image.width; ++j) {
            file.write((char*)image.pixels[i][j], 3); // Write R, G, B values
        }
    }

    file.close();
    return true;
}

// Function to subtract ambient light from the total illumination
void subtractAmbient(const PPMImage& totalImage, const PPMImage& ambientImage, PPMImage& resultImage) {
    for (int i = 0; i < totalImage.height; ++i) {
        for (int j = 0; j < totalImage.width; ++j) {
            for (int k = 0; k < 3; ++k) { // Loop through R, G, B channels
                int value = (int)totalImage.pixels[i][j][k] - (int)ambientImage.pixels[i][j][k];
                resultImage.pixels[i][j][k] = (unsigned char)(value < 0 ? 0 : value); // Clamp to 0 if negative
            }
        }
    }
}

// Function to display the menu
void displayMenu() {
    cout << "PPM File Reader/Writer" << endl;
    cout << "1. Read a PPM file" << endl;
    cout << "2. Write a PPM file" << endl;
    cout << "3. Isolate light contribution (Task 2)" << endl;
    cout << "4. Exit" << endl;
    cout << "Enter your choice: ";
}

int main() {
    PPMImage ambientImage, totalImage, resultImage;
    string filename;
    int choice;

    while (true) {
        displayMenu();
        cin >> choice;

        // Clear the input buffer
        cin.ignore(numeric_limits<streamsize>::max(), '\n');

        switch (choice) {
            case 1: // Read a PPM file
                cout << "Enter the PPM file name to read: ";
                getline(cin, filename); // Use getline to handle filenames with spaces
                if (readPPM(filename, ambientImage)) {
                    cout << "File read successfully!" << endl;
                    cout << "Image details:" << endl;
                    cout << "Magic Number: " << ambientImage.magicNumber << endl;
                    cout << "Width: " << ambientImage.width << ", Height: " << ambientImage.height << endl;
                    cout << "Max Color Value: " << ambientImage.maxColorValue << endl;
                    cout << "First pixel values (R, G, B): "
                         << (int)ambientImage.pixels[0][0][0] << ", "
                         << (int)ambientImage.pixels[0][0][1] << ", "
                         << (int)ambientImage.pixels[0][0][2] << endl;
                }
                break;

            case 2: // Write a PPM file
                if (ambientImage.magicNumber.empty()) {
                    cout << "Error: No image data loaded. Please read a PPM file first." << endl;
                    break;
                }
                cout << "Enter the output PPM file name: ";
                getline(cin, filename); // Use getline to handle filenames with spaces
                if (writePPM(filename, ambientImage)) {
                    cout << "File written successfully!" << endl;
                }
                break;

            case 3: // Isolate light contribution (Task 2)
                if (ambientImage.magicNumber.empty()) {
                    cout << "Error: Ambient light image not loaded. Please read a PPM file first." << endl;
                    break;
                }
                cout << "Enter the total illumination PPM file name: ";
                getline(cin, filename);
                if (readPPM(filename, totalImage)) {
                    // Allocate memory for the result image
                    resultImage.magicNumber = "P6";
                    resultImage.width = totalImage.width;
                    resultImage.height = totalImage.height;
                    resultImage.maxColorValue = totalImage.maxColorValue;
                    resultImage.pixels = allocatePixels(resultImage.width, resultImage.height);

                    // Subtract ambient light
                    subtractAmbient(totalImage, ambientImage, resultImage);

                    // Save the result
                    cout << "Enter the output PPM file name for the isolated light contribution: ";
                    getline(cin, filename);
                    if (writePPM(filename, resultImage)) {
                        cout << "Isolated light contribution saved successfully!" << endl;
                    }

                    // Free memory for the result image
                    freePixels(resultImage.pixels, resultImage.width, resultImage.height);
                }
                break;

            case 4: // Exit
                if (!ambientImage.magicNumber.empty()) {
                    freePixels(ambientImage.pixels, ambientImage.width, ambientImage.height); // Free memory before exiting
                }
                cout << "Exiting the program. Goodbye!" << endl;
                return 0;

            default:
                cout << "Invalid choice. Please try again." << endl;
                break;
        }
    }

    return 0;
}
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

// Function to change the color of a light source
void changeLightColor(PPMImage& image, float redScale, float greenScale, float blueScale) {
    for (int i = 0; i < image.height; ++i) {
        for (int j = 0; j < image.width; ++j) {
            image.pixels[i][j][0] = (unsigned char)(image.pixels[i][j][0] * redScale);   // Red channel
            image.pixels[i][j][1] = (unsigned char)(image.pixels[i][j][1] * greenScale); // Green channel
            image.pixels[i][j][2] = (unsigned char)(image.pixels[i][j][2] * blueScale); // Blue channel
        }
    }
}

// Function to create negative light effects
void createNegativeLight(const PPMImage& totalImage, const PPMImage& lightImage, PPMImage& resultImage) {
    for (int i = 0; i < totalImage.height; ++i) {
        for (int j = 0; j < totalImage.width; ++j) {
            for (int k = 0; k < 3; ++k) { // Loop through R, G, B channels
                int value = (int)totalImage.pixels[i][j][k] - (int)lightImage.pixels[i][j][k];
                resultImage.pixels[i][j][k] = (unsigned char)(value < 0 ? 0 : value); // Clamp to 0 if negative
            }
        }
    }
}

// Function to convert a color image to grayscale
void convertToGrayscale(const PPMImage& colorImage, PPMImage& grayscaleImage) {
    for (int i = 0; i < colorImage.height; ++i) {
        for (int j = 0; j < colorImage.width; ++j) {
            // Calculate grayscale value using the weighted sum formula
            unsigned char grayValue = (unsigned char)(
                0.299 * colorImage.pixels[i][j][0] + // Red channel
                0.587 * colorImage.pixels[i][j][1] + // Green channel
                0.114 * colorImage.pixels[i][j][2]   // Blue channel
            );

            // Set all three channels (R, G, B) to the grayscale value
            grayscaleImage.pixels[i][j][0] = grayValue;
            grayscaleImage.pixels[i][j][1] = grayValue;
            grayscaleImage.pixels[i][j][2] = grayValue;
        }
    }
}

// Function to compute the weighted average of two images
void weightedAverage(const PPMImage& imageA, const PPMImage& imageB, PPMImage& resultImage, float weight) {
    for (int i = 0; i < imageA.height; ++i) {
        for (int j = 0; j < imageA.width; ++j) {
            for (int k = 0; k < 3; ++k) { // Loop through R, G, B channels
                float value = weight * imageA.pixels[i][j][k] + (1 - weight) * imageB.pixels[i][j][k];
                resultImage.pixels[i][j][k] = (unsigned char)(value < 0 ? 0 : (value > 255 ? 255 : value)); // Clamp to 0-255
            }
        }
    }
}

// Function to display the color options
void displayColorMenu() {
    cout << "Select a color to apply:" << endl;
    cout << "1. Red" << endl;
    cout << "2. Green" << endl;
    cout << "3. Blue" << endl;
    cout << "Enter your choice: ";
}

// Function to display the main menu
void displayMenu() {
    cout << "PPM File Reader/Writer" << endl;
    cout << "1. Read a PPM file" << endl;
    cout << "2. Write a PPM file" << endl;
    cout << "3. Isolate light contribution (Task 2)" << endl;
    cout << "4. Change light color (Task 3)" << endl;
    cout << "5. Create negative light effects (Task 4)" << endl;
    cout << "6. Weighted Average of Two Images (Task 5)" << endl;
    cout << "7. Convert to Grayscale (Task 6)" << endl;
    cout << "8. Exit" << endl;
    cout << "Enter your choice: ";
}

int main() {
    PPMImage ambientImage, totalImage, resultImage, negativeImage, imageA, imageB, grayscaleImage;
    string filename;
    int choice;

    // Declare scaling factors outside the switch statement
    float redScale = 0.0, greenScale = 0.0, blueScale = 0.0;

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
                }
                break;

            case 4: // Change light color (Task 3)
                if (resultImage.magicNumber.empty()) {
                    cout << "Error: No isolated light contribution loaded. Please perform Task 2 first." << endl;
                    break;
                }

                // Display color options
                displayColorMenu();
                int colorChoice;
                cin >> colorChoice;

                // Clear the input buffer after reading the color choice
                cin.ignore(numeric_limits<streamsize>::max(), '\n');

                // Set scaling factors based on the selected color
                switch (colorChoice) {
                    case 1: // Red
                        redScale = 1.0;
                        greenScale = 0.0;
                        blueScale = 0.0;
                        break;
                    case 2: // Green
                        redScale = 0.0;
                        greenScale = 1.0;
                        blueScale = 0.0;
                        break;
                    case 3: // Blue
                        redScale = 0.0;
                        greenScale = 0.0;
                        blueScale = 1.0;
                        break;
                    default:
                        cout << "Invalid choice. No color change applied." << endl;
                        break;
                }

                // Change the light color
                changeLightColor(resultImage, redScale, greenScale, blueScale);

                // Save the result
                cout << "Enter the output PPM file name for the modified light color: ";
                getline(cin, filename);
                if (writePPM(filename, resultImage)) {
                    cout << "Modified light color saved successfully!" << endl;
                }
                break;

            case 5: // Create negative light effects (Task 4)
                if (resultImage.magicNumber.empty()) {
                    cout << "Error: No isolated light contribution loaded. Please perform Task 2 first." << endl;
                    break;
                }

                // Allocate memory for the negative image
                negativeImage.magicNumber = "P6";
                negativeImage.width = totalImage.width;
                negativeImage.height = totalImage.height;
                negativeImage.maxColorValue = totalImage.maxColorValue;
                negativeImage.pixels = allocatePixels(negativeImage.width, negativeImage.height);

                // Create negative light effects
                createNegativeLight(totalImage, resultImage, negativeImage);

                // Save the result
                cout << "Enter the output PPM file name for the negative light effect: ";
                getline(cin, filename);
                if (writePPM(filename, negativeImage)) {
                    cout << "Negative light effect saved successfully!" << endl;
                }

                // Free memory for the negative image
                freePixels(negativeImage.pixels, negativeImage.width, negativeImage.height);
                break;

            case 6: // Weighted Average of Two Images (Task 5)
                cout << "Enter the first PPM file name (Image A): ";
                getline(cin, filename);
                if (!readPPM(filename, imageA)) {
                    break;
                }

                cout << "Enter the second PPM file name (Image B): ";
                getline(cin, filename);
                if (!readPPM(filename, imageB)) {
                    break;
                }

                // Check if the images have the same dimensions
                if (imageA.width != imageB.width || imageA.height != imageB.height) {
                    cout << "Error: The two images must have the same dimensions." << endl;
                    break;
                }

                // Allocate memory for the result image
                resultImage.magicNumber = "P6";
                resultImage.width = imageA.width;
                resultImage.height = imageA.height;
                resultImage.maxColorValue = imageA.maxColorValue;
                resultImage.pixels = allocatePixels(resultImage.width, resultImage.height);

                // Generate a sequence of images with varying weights
                for (int i = 0; i <= 10; ++i) {
                    float weight = i * 0.1f; // Vary weight from 0.0 to 1.0 in steps of 0.1
                    weightedAverage(imageA, imageB, resultImage, weight);

                    // Save the result
                    string outputFilename = "morph_" + to_string(i) + ".ppm";
                    if (writePPM(outputFilename, resultImage)) {
                        cout << "Saved: " << outputFilename << endl;
                    }
                }

                // Free memory for the result image
                freePixels(resultImage.pixels, resultImage.width, resultImage.height);
                break;

            case 7: // Convert to Grayscale (Task 6)
                if (ambientImage.magicNumber.empty()) {
                    cout << "Error: No image data loaded. Please read a PPM file first." << endl;
                    break;
                }

                // Allocate memory for the grayscale image
                grayscaleImage.magicNumber = "P6";
                grayscaleImage.width = ambientImage.width;
                grayscaleImage.height = ambientImage.height;
                grayscaleImage.maxColorValue = ambientImage.maxColorValue;
                grayscaleImage.pixels = allocatePixels(grayscaleImage.width, grayscaleImage.height);

                // Convert the image to grayscale
                convertToGrayscale(ambientImage, grayscaleImage);

                // Save the result
                cout << "Enter the output PPM file name for the grayscale image: ";
                getline(cin, filename);
                if (writePPM(filename, grayscaleImage)) {
                    cout << "Grayscale image saved successfully!" << endl;
                }

                // Free memory for the grayscale image
                freePixels(grayscaleImage.pixels, grayscaleImage.width, grayscaleImage.height);
                break;

            case 8: // Exit
                if (!ambientImage.magicNumber.empty()) {
                    freePixels(ambientImage.pixels, ambientImage.width, ambientImage.height); // Free memory before exiting
                }
                if (!resultImage.magicNumber.empty()) {
                    freePixels(resultImage.pixels, resultImage.width, resultImage.height); // Free memory before exiting
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
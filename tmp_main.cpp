#include<iostream>
#include<fstream>
#include<string>
#include<vector>

using namespace std;

struct colorNode
{
    unsigned char red;
    unsigned char green;
    unsigned char blue;

    void makeRed()
    {
        green = 0;
        blue = 0;
    }
    void makeGreen()
    {
        red = 0;
        blue = 0;
    }
    void makeBlue()
    {
        green = 0;
        red = 0;
    }
    void makeCyan()
    {
        red = 0;
    }
};

class picture
{
public:
    vector<unsigned char> red;
    vector<unsigned char> green;
    vector<unsigned char> blue;
    string fileName;
    string line;
    string magicNumber;
    string width;
    string height;
    string maxColorVal;
    int count = 0;
    ifstream file;

public:
    void getFileName(const string& prompt)
    {
        cout << prompt;
        cin >> fileName;
    }

    void readImage()
    {
        file.open(fileName, ios::binary);
        if (file.is_open())
        {
            cout << "File Opened Successfully\n";
            do
            {
                getline(file, line);
                if (line[0] == '#')
                {
                    cout << "Comment Found!" << endl;
                }
                else
                {
                    count++;
                    switch (count)
                    {
                    case 1:
                        magicNumber = line;
                        break;
                    case 2:
                        for (int i = 0; i < line.length(); i++)
                        {
                            if (line[i] != ' ')
                            {
                                width += line[i];
                            }
                            else
                            {
                                height = line.substr(i + 1, line.length() - 1);
                                break;
                            }
                        }
                        break;
                    case 3:
                        maxColorVal = line;
                        break;
                    }
                }
            } while (maxColorVal == "");

            colorNode c;
            for (int i = 0; i < stoi(width); i++)
            {
                for (int j = 0; j < stoi(height); j++)
                {
                    file.read((char *)&c, sizeof(colorNode));
                    red.push_back(c.red);
                    green.push_back(c.green);
                    blue.push_back(c.blue);
                }
            }
            cout << "Magic Number: " << magicNumber << endl;
            cout << "Width: " << stoi(width) << endl;
            cout << "Height: " << stoi(height) << endl;
            cout << "Maximum Color Value: " << maxColorVal << endl;
        }
        else
        {
            cout << "Failed to Open File: " << fileName << endl;
        }
    }

    void writeImage(const string& outputFileName)
    {
        ofstream outFile(outputFileName, ios::binary);
        if (outFile.is_open())
        {
            outFile << magicNumber << endl;
            outFile << width << " " << height << endl;
            outFile << maxColorVal << endl;
            colorNode c;
            for (int i = 0; i < red.size(); i++)
            {
                c.red = red[i];
                c.green = green[i];
                c.blue = blue[i];
                outFile.write((char *)&c, sizeof(colorNode));
            }
            cout << "Image successfully written to: " << outputFileName << endl;
        }
        else
        {
            cout << "Failed to write to file: " << outputFileName << endl;
        }
    }
};

void displayMenu()
{
    cout << "====================================" << endl;
    cout << "          Image Processing Menu     " << endl;
    cout << "====================================" << endl;
    cout << "1. Read Image File" << endl;
    cout << "2. Write Image File" << endl;
    cout << "3. Produce Negative Image" << endl;
    cout << "4. Produce Grayscale Image" << endl;
    cout << "5. Produce Single-Colored Image" << endl;
    cout << "6. Subtract Ambient Image" << endl;
    cout << "7. Exit" << endl;
    cout << "====================================" << endl;
}

void makeNegative(picture &p1)
{
    string outputFileName;
    cout << "Enter the output file name (with extension): ";
    cin >> outputFileName;

    ofstream outFile(outputFileName, ios::binary);
    if (outFile.is_open())
    {
        outFile << p1.magicNumber << endl;
        outFile << p1.width << " " << p1.height << endl;
        outFile << p1.maxColorVal << endl;
        colorNode c;
        for (int i = 0; i < p1.red.size(); i++)
        {
            c.red = (unsigned char)((int)stoi(p1.maxColorVal) - (int)p1.red[i]);
            c.green = (unsigned char)((int)stoi(p1.maxColorVal) - (int)p1.green[i]);
            c.blue = (unsigned char)((int)stoi(p1.maxColorVal) - (int)p1.blue[i]);
            outFile.write((char *)&c, sizeof(colorNode));
        }
        cout << "Negative image saved to: " << outputFileName << endl;
    }
    else
    {
        cout << "Failed to save negative image." << endl;
    }
}

void makeGreyScale(picture &p1)
{
    string outputFileName;
    cout << "Enter the output file name (with extension): ";
    cin >> outputFileName;

    ofstream outFile(outputFileName, ios::binary);
    if (outFile.is_open())
    {
        outFile << "P5" << endl;
        outFile << p1.width << " " << p1.height << endl;
        outFile << p1.maxColorVal << endl;
        unsigned char gray;
        for (int i = 0; i < p1.red.size(); i++)
        {
            gray = (unsigned char)(0.299 * p1.red[i] + 0.587 * p1.green[i] + 0.114 * p1.blue[i]);
            outFile.write((char *)&gray, sizeof(unsigned char));
        }
        cout << "Grayscale image saved to: " << outputFileName << endl;
    }
    else
    {
        cout << "Failed to save grayscale image." << endl;
    }
}

void singleColored(picture &p1)
{
    cout << "Which Color You want to keep?" << endl;
    cout << "1. Red" << endl;
    cout << "2. Blue" << endl;
    cout << "3. Green" << endl;
    cout << "4. Cyan (Keeping only Green and Blue Color)" << endl;
    int choice;
    cout << "Enter your Choice: ";
    cin >> choice;

    string outputFileName;
    cout << "Enter the output file name (with extension): ";
    cin >> outputFileName;

    ofstream outFile(outputFileName, ios::binary);
    if (outFile.is_open())
    {
        outFile << p1.magicNumber << endl;
        outFile << p1.width << " " << p1.height << endl;
        outFile << p1.maxColorVal << endl;
        colorNode c;
        switch (choice)
        {
        case 1:
            for (int i = 0; i < p1.red.size(); i++)
            {
                c.red = p1.red[i];
                c.blue = 0;
                c.green = 0;
                outFile.write((char *)&c, sizeof(colorNode));
            }
            break;
        case 2:
            for (int i = 0; i < p1.red.size(); i++)
            {
                c.red = 0;
                c.blue = p1.blue[i];
                c.green = 0;
                outFile.write((char *)&c, sizeof(colorNode));
            }
            break;
        case 3:
            for (int i = 0; i < p1.red.size(); i++)
            {
                c.red = 0;
                c.blue = 0;
                c.green = p1.green[i];
                outFile.write((char *)&c, sizeof(colorNode));
            }
            break;
        case 4:
            for (int i = 0; i < p1.red.size(); i++)
            {
                c.blue = p1.blue[i];
                c.green = p1.green[i];
                c.red = 0;
                outFile.write((char *)&c, sizeof(colorNode));
            }
            break;
        }
        cout << "Colored image saved to: " << outputFileName << endl;
    }
    else
    {
        cout << "Failed to save colored image." << endl;
    }
}

void subImage(picture &p1, picture &ambient)
{
    string outputFileName;
    cout << "Enter the output file name (with extension): ";
    cin >> outputFileName;

    ofstream outFile(outputFileName, ios::binary);
    if (outFile.is_open())
    {
        outFile << p1.magicNumber << endl;
        outFile << p1.width << " " << p1.height << endl;
        outFile << p1.maxColorVal << endl;
        colorNode c;
        for (int i = 0; i < p1.red.size(); i++)
        {
            c.red = static_cast<unsigned char>(max(0, min(255, (int)p1.red[i] - (int)ambient.red[i])));
            c.green = static_cast<unsigned char>(max(0, min(255, (int)p1.green[i] - (int)ambient.green[i])));
            c.blue = static_cast<unsigned char>(max(0, min(255, (int)p1.blue[i] - (int)ambient.blue[i])));
            outFile.write((char *)&c, sizeof(colorNode));
        }
        cout << "Subtracted ambient image saved to: " << outputFileName << endl;
    }
    else
    {
        cout << "Failed to save subtracted ambient image." << endl;
    }
}

int main()
{
    picture p1, p2;
    int choice;
    bool exitProgram = false;

    while (!exitProgram)
    {
        displayMenu();
        cout << "Enter your choice: ";
        cin >> choice;

        switch (choice)
        {
        case 1: // Read Image File
            p1.getFileName("Enter the file name to read: ");
            p1.readImage();
            break;
        case 2: // Write Image File
        {
            string outputFileName;
            cout << "Enter the output file name (with extension): ";
            cin >> outputFileName;
            p1.writeImage(outputFileName);
            break;
        }
        case 3: // Produce Negative Image
            if (p1.red.empty())
            {
                cout << "No image loaded. Please read an image first." << endl;
            }
            else
            {
                makeNegative(p1);
            }
            break;
        case 4: // Produce Grayscale Image
            if (p1.red.empty())
            {
                cout << "No image loaded. Please read an image first." << endl;
            }
            else
            {
                makeGreyScale(p1);
            }
            break;
        case 5: // Produce Single-Colored Image
            if (p1.red.empty())
            {
                cout << "No image loaded. Please read an image first." << endl;
            }
            else
            {
                singleColored(p1);
            }
            break;
        case 6: // Subtract Ambient Image
            if (p1.red.empty())
            {
                cout << "No image loaded. Please read an image first." << endl;
            }
            else
            {
                p2.getFileName("Enter the ambient image file name: ");
                p2.readImage();
                subImage(p1, p2);
            }
            break;
        case 7: // Exit
            cout << "Exiting the program. Goodbye!" << endl;
            exitProgram = true;
            break;
        
        default:
            cout << "Invalid choice! Please try again." << endl;
            break;
        }
    }

    return 0;
}
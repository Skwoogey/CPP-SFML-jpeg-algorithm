// SFML-jpg.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include <iostream>
#include <cstring>
#include <SFML/Graphics.hpp>
#include <array>
#include <assert.h>
#include <iomanip>

#include "DynamicMultiArray.h"
#include "colours.h"
#include "DCT.h"
#include "JpegCompress.h"

byte face[8][8] =
{
    { 000, 000, 000, 255, 255, 000, 000, 000 },
    { 000, 000, 255, 000, 000, 255, 000, 000 },
    { 000, 000, 255, 000, 000, 255, 000, 000 },
    { 000, 000, 255, 000, 000, 255, 000, 000 },
    { 000, 000, 000, 255, 255, 000, 000, 000 },
    { 000, 255, 000, 000, 000, 000, 255, 000 },
    { 000, 000, 255, 255, 255, 255, 000, 000 },
    { 000, 000, 000, 000, 000, 000, 000, 000 }
};

void RGBAtoYCbCrImage(DynamicMultiArray<RGBA, 2>& from, DynamicMultiArray<YCbCr, 2>& to)
{
    assert(from.getSize() == to.getSize());

    std::array<int, 2> index;
    for (int d0 = 0; d0 < from.getSize()[0]; d0++)
    {
        for (int d1 = 0; d1 < from.getSize()[1]; d1++)
        {
            index = { d0, d1 };
            //std::cout << d0 << " " << d1 << '\n';
            //std::cout << from[index] << '\n';
            to[index] = YCbCr(from[index]);
            //std::cout << to[index] << "\n\n";
        }
    }
}

int main()
{
    sf::RenderWindow window(sf::VideoMode(600, 600), "SFML works!");
    sf::Texture texture;
    //texture.create(4, 4);

    DCT_2D dct;

    std::array<int, 2> arrayN = { 8, 8 };
    DynamicMultiArray<byte, 2> faceArray(arrayN);
    faceArray.copyFromMemory((byte*)face);

    auto encoded = dct.transform(faceArray);

    std::array<int, 2> index;
    std::cout << "DCT coefs:\n";
    for (int d0 = 0; d0 < DCT_N; d0++)
    {
        for (int d1 = 0; d1 < DCT_N; d1++)
        {
            index = { d0, d1 };
            std::cout << std::setw(4) << (int)encoded[index] << " ";
        }
        std::cout << '\n';
    }

    dct.restore(encoded);
    std::cout << "original image:\n";
    for (int d0 = 0; d0 < DCT_N; d0++)
    {
        for (int d1 = 0; d1 < DCT_N; d1++)
        {
            index = { d0, d1 };
            std::cout << (int)faceArray[index] << " ";
        }
        std::cout << '\n';
    }
    system("pause");

    sf::Image bmpFile;
    bmpFile.loadFromFile("sample_640×426.bmp");
    auto imageSize = bmpFile.getSize();
    std::array<int, 2> array3 = { imageSize.y, imageSize.x};
    DynamicMultiArray<RGBA, 2> imageRGBA(array3);
    DynamicMultiArray<YCbCr, 2> imageYCbCr(array3);

    imageRGBA.copyFromMemory(reinterpret_cast<const RGBA*>(bmpFile.getPixelsPtr()));


    // convert RGB int YCbCr space
    RGBAtoYCbCrImage(imageRGBA, imageYCbCr);

    // downsample chrominance
    std::function<void(YCbCr&)> func = [](YCbCr& colour) {
        int oldValue = colour.Y;
        colour.Y >>= 1;
        //std::cout << oldValue << " " << (int)colour.Y << "\n";
    };

    imageYCbCr.forEach(func);
    
    //array3 = { 10, 10, 0 };   image[array3] = 0;
    //array3 = { 10, 10, 1 };   image[array3] = 0;
    //array3 = { 10, 10, 2 };   image[array3] = 0;

    //array3 = { 10, 12, 0 };   image[array3] = 0;
    //array3 = { 10, 12, 1 };   image[array3] = 0;
    //array3 = { 10, 12, 2 };   image[array3] = 0;

    //array3 = { 12, 10, 0 };   image[array3] = 0;
    //array3 = { 12, 10, 1 };   image[array3] = 0;
    //array3 = { 12, 10, 2 };   image[array3] = 0;

    //array3 = { 12, 12, 0 };   image[array3] = 0;
    //array3 = { 12, 12, 1 };   image[array3] = 0;
    //array3 = { 12, 12, 2 };   image[array3] = 0;


    texture.create(imageSize.x, imageSize.y);
    texture.update(reinterpret_cast<sf::Uint8*>(imageRGBA.get()));

    //sf::Uint8 imageArray[4][4][4] = { 255 };
    //std::memset(imageArray, 255, sizeof(imageArray));
    ////std::cout << (int)imageArray[4][4][3] << std::endl;
    //imageArray[1][1][0] = 0;
    //imageArray[1][1][1] = 0;

    //texture.update((const sf::Uint8*)imageArray);

    sf::Sprite sprite(texture);
    auto windowSize = window.getSize();
    auto textureSize = texture.getSize();
    //std::cout << textureSize.x << " " << textureSize.y << '\n';
    sprite.setScale(
        (float)windowSize.x / textureSize.x,
        (float)windowSize.y / textureSize.y
    );

    while (window.isOpen())
    {
        window.clear();
        window.draw(sprite);
        window.display();

        sf::Event event;
        while (window.pollEvent(event))
        {
            if (event.type == sf::Event::Closed)
                window.close();
        }
    }
    system("pause");

    return 0;
}

// Run program: Ctrl + F5 or Debug > Start Without Debugging menu
// Debug program: F5 or Debug > Start Debugging menu

// Tips for Getting Started: 
//   1. Use the Solution Explorer window to add/manage files
//   2. Use the Team Explorer window to connect to source control
//   3. Use the Output window to see build output and other messages
//   4. Use the Error List window to view errors
//   5. Go to Project > Add New Item to create new code files, or Project > Add Existing Item to add existing code files to the project
//   6. In the future, to open this project again, go to File > Open > Project and select the .sln file

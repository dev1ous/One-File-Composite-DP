# Composite-like leveraging CRTP
 
A tool, inspired from the Curriously Reccuring Template Pattern idiom, that i developed for my student projects this year. 
We used it to create a derived class which is used to create buttons quickly, efficiently and at the same time have access to functions, easy to use, to personalize them.
The idea was to benefit from the advantages of the composite design pattern, to eliminate virtual functions and having only one header, which makes it easy to integrate with existing project.

# Examples

All examples on how to use this tool and the different functions are provided in commentaries or in test.cpp file.

# Requirements

* You need a compiler who support C++17 language features and put the language standard on it in properties.
* It has been tested on Clang and the latest version of MSVC to date.
* You can include the SFML too, if you want to use the button class

# Credits

Developed by [@dev1ous](https://github.com/dev1ous)

Thanks to [@Tama-sama](https://github.com/Tama-sama) for various help( insane programmer ) and [@Predatorium](https://github.com/Predatorium) for his help to extend the functions his the button class during our student project ( he's a coding machine ).

# License

Under the MIT License, see LICENSE.txt for more information.

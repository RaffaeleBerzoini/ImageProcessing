# Basic Math Operations on .bmp images

- ###  Raffaele Berzoini  ([@RaffaeleBerzoini](https://github.com/RaffaeleBerzoini)) <br> raffaele.berzoini@mail.polimi.it


## Setup

- Download all the files and the folder in the repository
- Execute the following commands to obtain the .exe file
    ```shell
    >  gcc -c bmpv2.c
    >  gcc bmpv2.o main.c -o main
    ```

## Use of the application

As the .exe file starts you will find a READ_ME.txt file with the instrctions



The script was initially thought to run on Windows, for aestethic purposes ShellExecute() is called here and there.
Please feel free to remove all the lines in int main() in main.c containing the ShellExecute() command if running on other OS.




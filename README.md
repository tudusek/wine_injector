# wine_injector
Special thanks to crwn1337 and casept for the original code

## usage
 ```
WINEDEBUG=-all wine wine_injector.exe [--edll dll.dll] [--ldll dll.dll] <exe> <optional args for exe>
```
You can load dll at start of the process using --edll (early load) or --ldll (late load) to load dll(s) after keypress.
You can use as much --edll/--ldll as you need

## building
1. clone the repo
    ```
    git clone https://github.com/crwn1337/wine_injector
    ```
2. cd into the directory
    ```
    cd wine_injector
    ```
3. make directory 'bin'
    ```
    mkdir bin
    ```
4. cd into 'bin'
    ```
    cd bin
    ```
5. generate cmake project
    ```
    cmake -DCMAKE_BUILD_TYPE=Release ..
    ```
6. build cmake project
    ```
    cmake --build .
    ```

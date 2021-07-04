# wine_injector

## usage
1. build the project first (see building)
2. run it:
    ```
    WINEDEBUG=-all wine wine_injector.exe <exe> <dll>
    ```
3. wait for the process to fully load and then press enter in the terminal

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

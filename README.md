# _Sisyph_


### Compile 
```Bash
$ sudo apt-get --yes install libboost-all-dev libcrypto++-dev libstdc++6 clang
$ make
```

### Docker
```Bash
$ sudo docker build --no-cache -t sisyph .
$ sudo docker run --rm -t -i sisyph
```
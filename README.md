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
container# git clone https://github.com/devmalic/_Sisyph_.git
container# cd _Sisyph_ ; make ; cd - ; mv _Sisyph_/bin/sisyph . ; rm -rf _Sisyph_
```
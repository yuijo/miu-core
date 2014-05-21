Miu
========

「ねぇ、新吾くん。蛇口が壊れた水道って、どうなるか知ってる？」


## Dependencies

* libzmq
* libmsgpack
* libboost_program_options


## Installation

```bash
git clone git@github.com:yuijo/miu-core.git
cd miu-core
mkdir -p build
cd build
cmake .. -DCMAKE_BUILD_TYPE=Release
make
make install
```


## Example

### miu-hub
```bash
miu hub -F tcp://127.0.0.1:22200 -B tcp://127.0.0.1:22201
```

### miu-cat
```bash
echo -n "hello miu" | miu cat -a tcp://127.0.0.1:22200 -t miu.cat -i 1
```

### miu-tail
```bash
miu tail -a tcp://127.0.0.1:22201
```


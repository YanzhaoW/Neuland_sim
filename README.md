## How to download this repository
Run following command in your terminal:
```text
git clone https://github.com/YanzhaoW/Neuland_sim.git
```


## How to run R3BRoot macros
NOTE: To continue the following steps, access to GSI servers is required.

### step 1:
Log in to a GSI analysis server (`lxlandana01` or `lxlandana02`) with your own account and run the following command:
```shell
source /u/land/fake_cvmfs/sourceme.sh
```
The command above cannot work in other GSI servers, such as the server you log in to just using `ssh username@lx-pool.gsi.de`.

### step 2:
Choose any folder and inside it download R3BRoot with
```shell
git clone https://github.com/R3BRootGroup/R3BRoot.git
cd R3BRoot
git checkout dev
git clone https://github.com/R3BRootGroup/macros.git
cd macros
```

### step 3:
Under the root folder `R3BRoot/`, compile the whole project with:
```text
mkdir build
cd build
cmake ..
make -j8
```
where the number `8` after `-j` in the last line represents the number of cores that are used for the compilation. You can choose any number of cores you would like.

If the compilation is successful, run the following command under the root folder `R3BRoot/`
```text
source ./build/config.sh
```

# MPC Package
## 1 Introduction
A C++ package for implementing basic MPC operations (including OT, secret share etc.) conveniently based on OpenMPI framework.

(☕️ I try to write C++ code in Java style to keep it readable)
## 2 Usage
### 2.1 OpenMPI Environment
#### 2.1.1 For macOS
- execute
`brew install openmpi`
#### 2.1.2 For Linux (Ubuntu)
- use source code to compile and install.
1. `wget https://download.open-mpi.org/release/open-mpi/v4.1/openmpi-4.1.5.tar.gz` (use same version for devices)
2. `tar -xzvf openmpi-_x._x._x.tar.gz`
3. `cd openmpi-_x._x._x`
4. `./configure --prefix=/usr/local/openmpi`
5. `make`
6. `sudo make install`
7. Add the following content into environment variable configuration file like `.bashrc`
    ```shell
    export PATH=/usr/local/openmpi/bin:$PATH
    export LD_LIBRARY_PATH=/usr/local/openmpi/lib:$LD_LIBRARY_PATH
    ```
8. `source ~/.bashrc`

### 2.2 Install this package
1. Download the whole project
2. `cd <project root>`
3. `sh install.sh`
> ⚠️ `install.sh` will delete former mpc_package.
### 2.3 Import this package in your project
CMakeLists.txt should contain the following content:
```txt
find_package(mpc_package REQUIRED)

target_link_directories(demo PUBLIC ${mpc_package_LIBRARY_DIRS})
target_link_libraries(demo mpc_package)
target_include_directories(demo PUBLIC ${mpc_package_INCLUDE_DIRS})
```
which will imports the package into your project.
### 2.4 Execute
1. Use the same username of Linux/macOS.
2. Set up non-password login between 2 machines. Using `ssh-keygen` and `ssh-copy-id`.
3. Place the executable file on the same path of 2 machines.
4. Edit a **hostfile.txt**, which records IP addresses of the 2 machines. (Or use host name by editing **/etc/hosts**) Write slot number for each machine.
   <br>(The format of host file can be referred in **hostfile.txt** in this project)
5. To distinguish executing machine, edit their hostnames by executing `sudo hostnamectl set-hostname ub1`. **ub1** is the new host name.
6. Your project should be complied by **CMake** instead of `mpicxx`. 
7. Then use `mpirun` to execute it. Here is an example shell:
    ```shell
    mpirun -np 2 -hostfile hostfile.txt a.out
    ```
## 3 Current Functions
- Arithmetic Share:
   - Multiplication Share with **RSA OT BMT** or **pre-generated BMT**
   - Addition Share 
- Boolean Share:
  - And Share with **RSA OT BMT** or **pre-generated BMT**
  - XOR Share
- Utility functions (under `utils` directory)
## 4 Test cases
All test case project will be compiled as `demo`. Please execute by:
```shell
cd <demo-project>
cmake .
make
mpirun -np 2 -hostfile hostfile.txt demo
```
### 4.1 Multiplication share
#### 4.1.1 With MTs through OT
- The code is under **test_cases/demo**.
- In this case, the MT is generated by process that MTs are generated by OT, illustrated in ABY paper (https://encrypto.de/papers/DSZ15.pdf).
- Here is an example output:
  ```shell
   [ub1:807754] [2024-07-27 14:50:48.051] [0] [ERROR] Beginning...
   [ub1:807754] [2024-07-27 14:50:48.051] [0] [ INFO] Multiplier: 1649254901
   [ub2:806961] [2024-07-27 14:50:47.907] [1] [ERROR] Beginning...
   [ub2:806961] [2024-07-27 14:50:47.907] [1] [ INFO] Multiplier: -1008493770
   [ub1:807754] [2024-07-27 14:50:57.295] [0] [ INFO] [Multiplication Share] OT RSA keys generation time: 4463 ms.
   [ub1:807754] [2024-07-27 14:50:57.295] [0] [ INFO] [Multiplication Share] OT RSA encryption time: 13 ms.
   [ub1:807754] [2024-07-27 14:50:57.295] [0] [ INFO] [Multiplication Share] OT RSA decryption time: 73 ms.
   [ub1:807754] [2024-07-27 14:50:57.295] [0] [ INFO] [Multiplication Share] OT MPI transmission and synchronization time: 4607 ms.
   [ub1:807754] [2024-07-27 14:50:57.295] [0] [ INFO] [Multiplication Share] OT total computation time: 9242 ms.
   [ub1:807754] [2024-07-27 14:50:57.295] [0] [ INFO] [Multiplication Share] Triple computation time: 9244 ms.
   [ub2:806961] [2024-07-27 14:50:57.150] [1] [ INFO] [Multiplication Share] OT RSA keys generation time: 4559 ms.
   [ub2:806961] [2024-07-27 14:50:57.151] [1] [ INFO] [Multiplication Share] OT RSA encryption time: 17 ms.
   [ub2:806961] [2024-07-27 14:50:57.151] [1] [ INFO] [Multiplication Share] OT RSA decryption time: 77 ms.
   [ub2:806961] [2024-07-27 14:50:57.151] [1] [ INFO] [Multiplication Share] OT MPI transmission and synchronization time: 4505 ms.
   [ub2:806961] [2024-07-27 14:50:57.151] [1] [ INFO] [Multiplication Share] OT total computation time: 9243 ms.
   [ub2:806961] [2024-07-27 14:50:57.151] [1] [ INFO] [Multiplication Share] Triple computation time: 9244 ms.
   [ub1:807754] [2024-07-27 14:50:57.296] [0] [ INFO] [Multiplication Share] MPI transmission and synchronization time: 4608 ms.
   [ub1:807754] [2024-07-27 14:50:57.296] [0] [ INFO] [Multiplication Share] Entire computation time: 9245 ms.
   [ub1:807754] [2024-07-27 14:50:57.296] [0] [ INFO] 4172185774
   [ub1:807754] [2024-07-27 14:50:57.296] [0] [ INFO] Done.
   [ub2:806961] [2024-07-27 14:50:57.151] [1] [ INFO] [Multiplication Share] MPI transmission and synchronization time: 4505 ms.
   [ub2:806961] [2024-07-27 14:50:57.151] [1] [ INFO] [Multiplication Share] Entire computation time: 9244 ms.
   [ub2:806961] [2024-07-27 14:50:57.151] [1] [ INFO] 4172185774
   [ub2:806961] [2024-07-27 14:50:57.151] [1] [ INFO] Done.             
  ```
  In this case, multipliers are `1649254901` and `-1008493770` which represents binary data.
  The product should be `1649254901 * -1008493700 = -1663263292800466770`. After leaving out extra bits, it is `4172185774`, the same as computation getResult.
#### 4.1.2 With fixed MTs
- Similar to last case, but the MTs are fixed generated from `FixedMultiplicationTripleHolder`.
- Here is an example output:
  ```shell
   [ub1:821054] [2024-07-27 15:56:27.052] [0] [ INFO] Beginning...
   [ub1:821054] [2024-07-27 15:56:27.053] [0] [ INFO] Multiplier: 1898752007
   [ub2:820067] [2024-07-27 15:56:27.127] [1] [ INFO] Beginning...
   [ub2:820067] [2024-07-27 15:56:27.127] [1] [ INFO] Multiplier: -1158067851
   [ub1:821054] [2024-07-27 15:56:27.072] [0] [ INFO] [Multiplication Share] Triple computation time: 19 ms.
   [ub2:820067] [2024-07-27 15:56:27.147] [1] [ INFO] [Multiplication Share] Triple computation time: 20 ms.
   [ub1:821054] [2024-07-27 15:56:27.073] [0] [ INFO] [Multiplication Share] MPI transmission and synchronization time: 1 ms.
   [ub1:821054] [2024-07-27 15:56:27.073] [0] [ INFO] [Multiplication Share] Entire computation time: 20 ms.
   [ub1:821054] [2024-07-27 15:56:27.073] [0] [ INFO] 4196518451
   [ub1:821054] [2024-07-27 15:56:27.073] [0] [ INFO] Done.
   [ub2:820067] [2024-07-27 15:56:27.148] [1] [ INFO] [Multiplication Share] MPI transmission and synchronization time: 1 ms.
   [ub2:820067] [2024-07-27 15:56:27.148] [1] [ INFO] [Multiplication Share] Entire computation time: 21 ms.
   [ub2:820067] [2024-07-27 15:56:27.148] [1] [ INFO] 4196518451
   [ub2:820067] [2024-07-27 15:56:27.148] [1] [ INFO] Done.
  ```
  The validation is omitted.
  



# Mining_pool-and-Miner
 两个工程文件夹mining pool 和miner,可以在一台计算机上同时运行两（多）个程序，或多台计算机上运行
 
 矿池与矿机使用socket网络通信，若一台计算机运行mining pool程序（作为矿池），其他计算机运行miner程序，只需要在miner的main.c中修改ip地址(serverAddr.sin_addr.s_addr)为矿池计算机的ip地址。
 
 矿池与矿机的通信方式见"矿池思路.txt"文件。
 
 支持矿机动态接入矿池。
一个矿机接入时结果：

![image](https://user-images.githubusercontent.com/69345371/115651824-8e06dd80-a35e-11eb-9bb3-269b8077fddf.png)

两个矿机接入时结果：

![image](https://user-images.githubusercontent.com/69345371/115651859-a2e37100-a35e-11eb-9e3e-cc64f648437f.png)

编译环境win10 dev c++，
用到pthread多线程编程，配置方式https://www.pianshen.com/article/1850308597/

修改编译选项：

![image](https://user-images.githubusercontent.com/69345371/115651712-4a13d880-a35e-11eb-917a-685f44cceca3.png)

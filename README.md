# SpaceEngine 0.990 详细信息生成器《星示昭明》(第二版半成品)

协议：[GPLv2](https://www.gnu.org/licenses/licenses.html)

上一个详细信息生成器链接：[百度贴吧搬运链接，SE论坛的原链接不知去向](https://tieba.baidu.com/p/4458618242)

详细信息生成器最初为SE官方论坛的用户Amwhere所作，这个工程为上述版本的C++重写，适配0.990以上版本，基于[CSpaceEngine](https://github.com/StellarDX/CSpaceEngine-Project)，
默认输出格式为Github Markdown(.md)，~~因为作者不会网页前端，不过以后可能会支持~~ 现在已经支持HTML了。
(目前本工具已放弃使用Visual Studio开发，新的工具是CMake。同时做了前后端分离，所以现在可以自由调整生成结果的排版和展示风格了。)

## 安装方法

1. 安装[Boost](https://www.boost.org/)，[fmtlib](https://fmt.dev/)和[CSpaceEngine](https://github.com/StellarDX/CSpaceEngine-Project)
2. 配置CMake环境：
```
cmake -S <源码解压目录> -B <中间目录> -DCMAKE_INSTALL_PREFIX:PATH=<安装目录> -DBoost_DIR:PATH=<Boost安装目录> -DFMT_FIND_DIRECTORY:PATH=<fmtlib安装目录> -DCSE_DIR:PATH=<CSE根目录>
```
3. 编译安装
```
cmake --build <中间目录> --target all install
```

## 使用方法

先说一下，这软件是没有图形界面的。
1. 打开SE，选中目标系统
2. 返回主菜单，点开工具->导出星球脚本，导出行星系统
3. 在导出的目录(通常是SE目录/export)下找到导出的系统文件
4. 转到生成器的目录，直接把文件拖入主程序。或者用命令行打开该目录，输入以下命令(两种都可以)：
```
InfoGen [options] <source-file>
InfoGen [options] -S <source-file> -B <path-to-output>
```
更多帮助信息输入`InfoGen -h`或`InfoGen --help`以查看

----------
*正因为有些事不能挽回也不能改变，人间才会有情感。 ——纳西妲*

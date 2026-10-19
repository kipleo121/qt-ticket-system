# 源码整理与运行说明

本项目为 Qt 票务系统课程小组项目。此副本用于上传源码，原始 Air_Final.zip 未修改。

## 本次整理

- 保留 35 个原始源码、UI、工程、图片资源与 SQL 文件。
- 排除 build/ 下的编译产物和缓存，以及 .qtcreator/ 下的个人 IDE 配置。
- dbhelper.h：本机数据库地址及连接配置改为从环境变量读取。
- AdminLoginDialog.cpp：删除写死的管理员密码，改读 AIR_ADMIN_PASSWORD；未配置时拒绝管理员登录。
- 除上述两个文件外，其余 33 个文件与压缩包内文件逐字节一致。
- 未提供新的 README.md 或 .gitignore，复制到仓库时保留你已有的这两个文件。

## 运行条件

上传源码不需要 Qt。重新编译运行时，需要安装 Qt 开发环境及 C++ 编译器。

- 工程入口：Airrr.pro，使用 qmake、C++17、Qt Widgets 和 Qt SQL。
- 原压缩包含 Qt 6.9.3 / 6.10.0、MinGW 64 位的构建目录，可作为恢复环境的参考；本副本未重新编译运行。
- 数据库代码使用 QODBC；SQL 脚本采用 MySQL 语法。需准备 MySQL、匹配位数的 MySQL ODBC 驱动及 Qt QODBC 插件。
- resources.qrc 引用 fly.png 和 basepic.png，请保留这两个文件。

## 数据库初始化

airdb_new.sql 包含表结构和航班示例数据。脚本会删除并重建 airdatabase 中的 Users、Flights、Orders 表，只应在专门的测试数据库中执行。

## 环境变量

在 Qt Creator 的项目运行环境中设置下列变量；普通 .env 文件不会被本程序自动读取。

| 变量 | 内容 |
|---|---|
| AIR_DB_CONNECTION | 已配置的 ODBC DSN 名称，或适合本机 ODBC 驱动的连接字符串 |
| AIR_DB_HOST | 数据库主机，默认 localhost |
| AIR_DB_USER | 数据库用户名，可选；视 DSN/连接字符串配置而定 |
| AIR_DB_PASSWORD | 数据库密码，可选；视 DSN/连接字符串配置而定 |
| AIR_ADMIN_PASSWORD | 自行设置的非空管理员密码 |

不要把自己的密码写回源码。发布副本改变了配置方式，原先写死的管理员密码不再有效。

## 当前验证范围

仅检查了文件完整性、工程/资源引用和配置替换，未安装 Qt、未连接数据库、未运行 SQL。此整理不是完整功能或安全审查。原项目的用户密码仍按原逻辑存储和比较，仅用于课程演示，若用于实际服务需另行完善认证与密码存储。

## 简历展示

请在已有 README 中补充界面截图、实际完成的功能、运行步骤，以及你在小组中的具体贡献。可链接本文件作为环境配置说明。
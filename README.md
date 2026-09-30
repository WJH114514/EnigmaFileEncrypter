# EnigmaFileEncrypter
C语言实现的文件十六进制数据Enigma加密

## 功能
- 预计算映射表，十六进制数既是数组下标，也是数据。
- 读取文件数据，每字节解析为两个十六进制数。
- 按照十六位三转子Enigma机进行加密。
- 创建带有.ngm后缀的加密文件，并写入数据。

## 使用
```bash
gcc ./EFE.c -o ./EFE
./EFE

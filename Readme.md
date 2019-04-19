## 修改aliIot.c
```
int main() {
    char *productKey = "a1GvAwy4lNt";
    char *deviceName = "pos";
    char *deviceSecret = "deviceSecret";
    connectIot(productKey, deviceName, deviceSecret);

    return 1;
}

```
- productKey  设备产品key
- deviceName  设备名称
- deviceSecret 设备密钥

将 productKey  deviceName  deviceSecret  替换为自己的值.

## 执行 cmake install 生成可执行程序
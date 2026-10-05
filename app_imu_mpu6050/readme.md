# Sensor Acelerometro MPU6050

O MPU-6050 é um sensor de movimento que integra um acelerômetro e um giroscópio de 3 eixos cada (totalizando 6 graus de liberdade). Muito utilizado em robótica, drones e realidade virtual, ele opera via comunicação I²C e possui um sensor de temperatura embutido, sendo ideal para calcular orientação e estabilidade.

- Tensão de operação: 3V a 5V
- Alcance do acelerômetro: configurável de ± 2g a ± 16g
- Alcance do giroscópio: configurável de ± 250°/s a ± 2000°/s
- Endereçamento I²C padrão: 0x68 (pode ser alterado para 0x69 conectando o pino ADO a 3.3V)


# Possíveis Problemas

```
Traceback (most recent call last):
  File "/home/vri/felipe/teste.py", line 5, in <module>
    mpu6050 = mpu6050.mpu6050(0x68)
  File "/home/vri/.local/lib/python3.10/site-packages/mpu6050/mpu6050.py", line 70, in __init__
    self.bus = smbus.SMBus(bus)
PermissionError: [Errno 13] Permission denied
```

O PermissionError: [Errno 13] ocorre porque sua conta de usuário não possui as permissões de leitura/gravação necessárias para acessar o barramento hardware /dev/i2c. Você pode corrigir isso rapidamente executando seu script com sudo ou adicionando permanentemente seu usuário ao grupo i2c.

1. Alterando as permissões do barramento
```bash
sudo chmod a+rw /dev/i2c-*
```

ou 

2. Adicionando o usuario no grupo i2c. Requer deslogar e logar novamente para efetivamente o usuario entrar no grupo
```bash
sudo usermod -aG i2c $USER
```

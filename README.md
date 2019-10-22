# Web proxy

Trabalho final da matéria de Teleinformática e Redes II do 2o semestre de 2017.

### Instalação

```
make
```

### Execução

```
./proxy <numero da porta de acesso>
```

### Log

O log do sitema esta dividido em algumas áreas seguindo o padrão a baixo, o log se encotra no arquivo resources/log.txt e cada mensagem de log esta separadas por quebra de linha.
Cada linha começa com uma letra que define o tipo da mensagem.

```
* r - Requisção ~ [tempo]	<cliente>	<método HTTP> 	-	<hospedeiro><caminho>
* b - Acesso bloqueado ~ [tempo]		<cliente>	-	<hospedeiro><caminho>	<motivo do bloqueio>
* c - cache ~ [tempo] 	<client>	- <hospedeiro><caminho>	-	<motivo>
* h - http ~ [tempo]	<hospedeiro><caminho>	<codigo http>	-	<cliente>
```

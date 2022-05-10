# Descrição do ambiente de testes

Para testes, foi utilizado Debian 9 rodando na plataforma _Windows Linux Subsystem_ (WSL), em uma máquina Intel i5 de nona geração, 32gb de memória, 128gb de disco SSD e 1tb de disco óptico. O projeto foi estruturado através da ferramenta _CMake_ e compilado com o _GCC 8.3_.

# Implementação

## Algoritmo de Eleição

A implementação do algoritmo de eleição (bully) foi implementada para ser executada após a detecção de um timeout de heartbeats do servidor principal. O server primario dispara pacotes election para os secundarios. As instancias secundarias possuem um timeout para o recebimento dos pacotes. Caso alguma delas tenha esse timeout, ela iniciará um processo de eleição mandando mensagens election para os servidores com ID maior que o seu e verificando se vai receber um pacote reply. Caso não receba pacote reply, entenderá que deve se tornar o servidor primario e inicia o 'Bully'. O bully envia packets coordinator contendo ip e porta para os outros servidores e também para os clients conectados que atualizarão seu novo servidor primário.


## Replicação Passiva

Para a replicação passiva, criamos uma nova estrutura ReplicaManager, onde armazenamos os sockets de conexão de servidores criados, bem como o gerenciamento de qual servidor é o primário, e as funções responsáveis para fazer o broadcast para os servidores secundários

## Principais estruturas e funções

### Server

O código do servidor se baseia na classe `Server`. Quando essa classe é instanciada, é feito o _bind_ de um _socket_ UDP. Quando o método `Start` é invocado, são lançadas duas _threads_: uma _thread_ executando a função `Server::Listen`, que despacha datagramas UDP em _threads_ que executam a função `Server::MessageHandler`, e uma _thread_ executando a função `Server::PendingNotificationWorker`, que processa as notificações pendentes de envio pelo servidor.

A função `Server::Listen` executa a chamada de _socket_ `recvfrom` até a variável `isListening` da classe `Server` for setada como falso. Cada vez que um datagrama for recebido pelo `recvfrom`, o _payload_ do datagrama é convertido para a estrutura `Message::Packet`, e uma nova _thread_ é lançada com a função `Server::MessageHandler`, passando o pacote e o endereço do remetente como parâmetro.

A função `Server::MessageHandler` essencialmente verifica o tipo da mensagem recebida, e executa a lógica apropriada:

1. Conectar um usuário
2. Desconectar um usuário
3. Seguir um usuário
4. Enviar uma notificação

Adicionalmente, os datagramas para seguir um usuário ou enviar uma notificação identificam o usuário (a estrutura `Profile`) pelo _host_ (endereço IP e porta).

A estrutura `Session` define os pontos onde um usuário está conectado. Possui um `Profile*` para identificar um usuário já existente, e um `std::vector<struct sockaddr_in>` para definir os locais de conexão. Essa estrutura é referenciada tanto pelo `SessionManager` quanto pelo `Profile`.

O gerenciador de sessões (`SessionManager`) guarda as sessões em um mapeamento do nome do usuário para a sessão. O gerenciador permite a busca da sessão tanto pelo nome do usuário, quanto pelo _host_.

O gerenciador de perfis (`ProfileManager`) guarda os perfis (`Profile`) em uma lista. Permite a busca pelo nome de usuário, salva os usuários e seus seguidores em disco, e permite que a lista de perfis seja reconstruída por um arquivo em disco.

### Client

O _client_ é implementado através da classe `Client`. Essa classe é instanciada com o nome do usuário que quer conectar, e o IP e porta do servidor. Ao chamar o método `Connect`, o _client_ abre um _socket_ e enviar uma mensagem de conexão para o servidor. Em seguida, lança uma _thread_ que processa datagramas recebidos do servidor.

O método `Client::Listen` recebe datagramas do _socket_ conectado com o servidor, e os processa de acordo com o tipo da mensagem. As mensagems recebidas do servidor podem ser:

1. Aceitar conexão
2. Rejeitar conexão
3. Notificação
4. Erro
5. Informação

Quando o _client_ recebe uma mensagem de rejeição de conexão, ele é automaticamente terminado.

### Messaging

Para a comunicação entre o _client_ e o _server_, foi implementado uma estrutura `Message::Packet` que tem 9 tipos diferentes:

1. Conectar um usuário (`PACKET_CONNECT_CMD`)
2. Desconectar um usuário (`PACKET_DISCONNECT_CMD`)
3. Aceitar a conexão de um usuário (`PACKET_ACCEPT_CONN_CMD`)
4. Rejeitar a conexão de um usuário (`PACKET_REJECT_CONN_CMD`)
5. Seguir um usuário (`PACKET_FOLLOW_CMD`)
6. Enviar uma notificação (cliente para servidor) (`PACKET_SEND_CMD`)
7. Receber uma notificação (servidor para cliente) (`PACKET_NOTIFICATION`)
8. Mensagem de erro (`PACKET_ERROR`)
9. Mensagem de informação (`PACKET_INFO`)
10. Requisitar informação do usuário (`PACKET_REQUEST_USER_INFO`)
11. Verificar se o servidor primário está respondendo (`PACKET_HEARTBEAT`)
12. Requisitar a eleição de um novo servidor primário (`PACKET_ELECTION`)
13. Responder a eleição de lider (`PACKET_REPLY`)
14. Comunicar a todos os processos o novo lider (`PACKET_COORDINATOR`)

A estrutura do pacote também tem número de sequência, um _payload_ e o seu tamanho, e o _timestamp_.

# Desafios

1. Fazer a sincronização de um novo servidor primário com os demais servers. Precisamos adaptar nossos pacotes para ter a informação do client(ip e porta) que enviava o pacote. Inicialmente faziamos o broadcast dos pacotes como definidos na etapa1 e isso gerou problemas quando as instancias secundarias se tornavam primárias.
2. Fazer a interface utilizando ncurses funcionar corretamente com a aplicação.
3. Tivemos problemas no algoritmo de eleição até que notamos que estavamos com diferentes threads acessando as mesmas variáveis, o que foi resolvido com um mutex.

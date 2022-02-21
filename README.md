Ideias de implementação

- Fila geral de notificações pendentes
  - Server recebe pedido de notificação do client
  - Busca na lista de usuários os usuários que seguem quem mandou a notificação
  - Pra cada um, adiciona uma notificação pendente numa lista global
  - Um worker fica pegando notificações dessa lista e processando elas.
    - Indo pela entidade do usuário, dá pra pegar uma lista de sessões ativas
    - Pela sessão ativa, pega o endereço de destino
    - Envia para cada sessão ativa, e incrementa um contador de envios
  - Um outro worker limpa a lista. Sempre que uma notificação tiver contador de envio > 0, tira ela da lista.
  - Sempre que o client conecta, ele recebe as notificações salvas que tem contador == 0
  - Se existem n sessões ativas, são n threads acessando a lista global, mais 1 thread de processamento, e 1 de limpeza
  - Problema (não tanto): thread de processamento e limpeza precisam percorrer toda lista.
    - Talvez ir limpando a medida que processa. Faz o processamento e já verifica se precisa tirar

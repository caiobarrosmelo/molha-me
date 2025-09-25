# molha-me
# Projeto IoT Plant Friends - Gamificação estilo Tamagotchi

Este projeto integra dispositivos IoT (ESP32) para monitoramento e irrigação automática de plantas, com uma interface gamificada estilo Tamagotchi para o usuário cuidar da planta como um pet virtual. A comunicação entre dispositivo e backend ocorre via HTTP(S).

---

## Estrutura do Projeto

```plaintext
iot-plant-pet/
├── README.md
├── .env                         # Variáveis de ambiente globais
├── docker-compose.yml           # Orquestração dos serviços (opcional)
├── docs/                        # Documentação complementar
│
├── firmware/                    # Código do dispositivo IoT (ESP32)
│   ├── src/
│   │   ├── main.ino             # Código principal Arduino
│   │   ├── config.h             # Configurações e definições
│   │   ├── http_client.cpp      # Comunicação HTTP com o backend
│   │   ├── sensors.cpp          # Leitura dos sensores
│   │   ├── actuators.cpp        # Controle de atuadores (relés, LEDs)
│   │   └── utils.cpp            # Funções auxiliares
│   └── lib/                     # Bibliotecas externas
│
├── backend/                     # API REST + lógica de negócio
│   ├── src/
│   │   ├── controllers/         # Endpoints da API (leituras, comandos, plantas)
│   │   ├── models/              # Modelos e esquemas do banco de dados
│   │   ├── services/            # Regras de negócio e gamificação
│   │   ├── routes/              # Definição das rotas da API
│   │   ├── db/                  # Migrações e conexão PostgreSQL
│   │   ├── app.js               # Configuração do servidor Express.js
│   │   └── server.js            # Inicialização do backend
│   ├── .env                     # Variáveis de ambiente para backend
│   └── package.json             # Dependências e scripts do Node.js
│
├── frontend/                    # Interface gamificada do usuário
│   ├── public/                  # Arquivos estáticos públicos (index.html, favicon)
│   ├── src/
│   │   ├── assets/              # Imagens, ícones e sons do mascote/planta
│   │   ├── components/          # Componentes reutilizáveis da interface
│   │   ├── screens/             # Telas da aplicação (Dashboard, Configurações)
│   │   ├── logic/               # Regras da gamificação (estado da planta, níveis)
│   │   ├── services/            # Comunicação HTTP com o backend (API)
│   │   ├── App.vue              # Componente raiz (para Vue.js)
│   │   └── main.js              # Inicialização do frontend
│   ├── package.json             # Dependências do frontend
│   └── vite.config.js           # Configuração do build
│
├── infra/                       # Infraestrutura e deployment
│   ├── docker/                  # Dockerfiles para backend e frontend
│   ├── nginx/                   # Configuração de proxy reverso (se necessário)
│   └── ci-cd/                   # Pipelines CI/CD (GitHub Actions, GitLab CI, etc.)
│
└── tests/                       # Testes automatizados
    ├── firmware/                # Testes simulados do firmware
    ├── backend/                 # Testes da API e regras de negócio
    └── frontend/                # Testes de interface (unitários e e2e)

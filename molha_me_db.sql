-- Tabela que registra os sensores disponíveis no sistema
CREATE TABLE sensores (
    id SERIAL PRIMARY KEY,                  	-- Identificador único do sensor
    tipo VARCHAR(50) NOT NULL,              	-- Tipo do sensor (ex: DHT22, sensor de solo)
    descricao VARCHAR(255),                 	-- Descrição detalhada do sensor
    unidade VARCHAR(50),                     	-- Unidade de medida do sensor (ex: %, °C)
    localizacao VARCHAR(100)                  	-- Localização física ou identificação do sensor
);

-- Tabela que armazena os atuadores que podem ser controlados
CREATE TABLE atuadores (
    id SERIAL PRIMARY KEY,                    	-- Identificador único do atuador
    tipo VARCHAR(50) NOT NULL,                	-- Tipo do atuador (ex: relé, bomba, válvula)
    descricao VARCHAR(255),                   	-- Descrição detalhada do atuador
    configuracao_json JSONB,                  	-- Configurações específicas do atuador em JSON (ex: parâmetros, timers)
    localizacao VARCHAR(100)                  	-- Localização física ou identificação do atuador
);

-- Tabela para registrar as leituras feitas pelos sensores
CREATE TABLE leituras_sensores (
    id SERIAL PRIMARY KEY,                    	-- Identificador único da leitura
    sensor_id INT REFERENCES sensores(id),   	-- FK para o sensor que realizou a leitura
    valor DECIMAL(10, 2),                     	-- Valor medido pelo sensor (com precisão decimal)
    data_leitura TIMESTAMP DEFAULT CURRENT_TIMESTAMP, -- Data e hora da leitura registrada
    CONSTRAINT fk_sensor FOREIGN KEY (sensor_id) REFERENCES sensores(id) -- Restrição de integridade referencial
);

-- Índice para acelerar consultas por sensor e data da leitura (muito usado para históricos)
CREATE INDEX idx_leituras_sensor_data ON leituras_sensores(sensor_id, data_leitura);

-- Tabela que registra comandos enviados para os atuadores
CREATE TABLE comandos_atuadores (
    id SERIAL PRIMARY KEY,                    	-- Identificador único do comando
    atuador_id INT REFERENCES atuadores(id), 	-- FK para o atuador a ser comandado
    estado BOOLEAN,                           	-- Estado desejado (true = ligar/ativar, false = desligar/desativar)
    data_comando TIMESTAMP DEFAULT CURRENT_TIMESTAMP, -- Data e hora do envio do comando
    duracao INT,                             	-- Duração do comando em segundos (se aplicável)
    status VARCHAR(50) DEFAULT 'pendente'   	-- Status do comando (ex: pendente, executado, falhou)
);

-- Tabela que contém as informações das plantas monitoradas/controladas
CREATE TABLE plantas (
    id SERIAL PRIMARY KEY,                    	-- Identificador único da planta
    nome VARCHAR(100) NOT NULL,               	-- Nome da planta (ex: Alface, Tomate)
    descricao VARCHAR(255),                    -- Descrição ou observações da planta
    umidade_soil_ideal DECIMAL(5, 2),         	-- Umidade ideal do solo para essa planta (em %)
    umidade_air_ideal DECIMAL(5, 2),          	-- Umidade ideal do ar para essa planta (em %)
    temperatura_ideal DECIMAL(5, 2),          	-- Temperatura ideal (em °C)
    luz_ideal VARCHAR(50),                     -- Necessidade ideal de luz (ex: baixa, média, alta)
    tipo_irrigacao VARCHAR(50)                 -- Tipo de irrigação (ex: gotejamento, aspersão)
);

-- Relacionamento N:N entre plantas e sensores, com configurações específicas para cada par
CREATE TABLE planta_sensor (
    id SERIAL PRIMARY KEY,                    	-- Identificador único da associação
    planta_id INT REFERENCES plantas(id),     	-- FK para a planta associada
    sensor_id INT REFERENCES sensores(id),   	-- FK para o sensor associado
    configuracao_json JSONB,                  	-- Configurações específicas do sensor para essa planta (ex: thresholds)
    status VARCHAR(50) DEFAULT 'ativo',      	-- Status da associação (ativo, inativo)
    ultima_atualizacao TIMESTAMP DEFAULT CURRENT_TIMESTAMP -- Data da última atualização dessa configuração
);

-- Relacionamento N:N entre plantas e atuadores, com configurações específicas para cada par
CREATE TABLE planta_atuador (
    id SERIAL PRIMARY KEY,                    	-- Identificador único da associação
    planta_id INT REFERENCES plantas(id),     	-- FK para a planta associada
    atuador_id INT REFERENCES atuadores(id),  	-- FK para o atuador associado
    configuracao_json JSONB,                  	-- Configurações específicas do atuador para essa planta (ex: tempo de ativação)
    status VARCHAR(50) DEFAULT 'ativo',      	-- Status da associação (ativo, inativo)
    ultima_atualizacao TIMESTAMP DEFAULT CURRENT_TIMESTAMP -- Data da última atualização dessa configuração
);


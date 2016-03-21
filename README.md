DETECÇÃO DE MOEDAS

Repositório que armazena código de detecção de moedas. O código conta o valor monetário mostrado na tela.

processamento-de-imgs/
- BlobDetector.cpp: Usado para detecção e tratamento de Blobs;
- Util.cpp: Calibragem da câmera (Hardware);

./
- settings.txt: Guarda as configurações a serem usadas pelo CoinCounter.cpp para identificar e separar as moedas;
- Calibrator.cpp: Configura o settings.txt;
- CoinCounter.cpp: main.

No momento os valores são lidos apenas em Reais. A ordem no Calibrator.cpp é hard-coded e a janela fecha após a calibragem. A interface para o usuário é extremamente minimalista. Pretendemos implementar:

1) Aumentar a flexibilidade do Calibrator.cpp, inclusive para outros e mais tipos de moeda;
2) Detectar outros nuances para diferenciação de moedas (cor, detalhes, grossura);
3) Adicionar elementos na interface (de ambos CoinCounter.cpp e Calibrator.cpp) para deixar o programa mais amigável ao usuário.

Se a cena for muito caótica ou a iluminação for fraca/inconstante o desempenho do programa não vai ser muito bom.

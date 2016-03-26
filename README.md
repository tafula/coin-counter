** DETECÇÃO DE MOEDAS **

Repositório que armazena código de detecção de moedas. O código conta o valor monetário mostrado na tela.

_processamento-de-imgs/_
- BlobDetector.cpp: Usado para detecção e tratamento de Blobs;
- Util.cpp: Calibragem da câmera (Hardware);

_./_
- settings.txt: Guarda as configurações a serem usadas pelo CoinCounter.cpp para identificar e separar as moedas;
- Calibrator.cpp: Configura o settings.txt;
- CoinCounter.cpp: main.

No momento os valores são lidos apenas em Reais. A ordem no Calibrator.cpp é hard-coded e a janela fecha após a calibragem. A interface para o usuário é extremamente minimalista. Pretendemos implementar:

- [ ] Aumentar a flexibilidade do Calibrator.cpp, inclusive para outros e mais tipos de moeda;
- [ ] Detectar outros nuances para diferenciação de moedas (cor, detalhes, grossura);
- [ ] Adicionar elementos na interface (de ambos CoinCounter.cpp e Calibrator.cpp) para deixar o programa mais amigável ao usuário.

O desempenho do programa fica melhor quando as moedas estão sobre uma superfície preta e fosca. O plano da câmera deve estar o mais paralelo o possível com o plano do background. A luz deve ser moderada e não muito caótica.

-----------------------
OBS:: Se você não estiver conseguindo dar make mesmo com todas as bibliotecas instaladas e atualizadas, uma das (ou ambas) coisas abaixo pode ajudar:

1. Em processamento-de-imgs/BlobDetector.hpp, tente trocar a linha 4 "#define labNote 0" para "#define labNote 1";
2. No Makefile, tente remover das LIBS (linha 4) a string "-isystem/usr/include".

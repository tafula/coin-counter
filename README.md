**DETECÇÃO DE MOEDAS**

Repositório que armazena código de detecção de moedas. O código conta o valor monetário mostrado na tela.

_processamento-de-imgs/_
- ToposDetector.cpp: Corrige diferença de tamanho na detecção de moedas num plano inclinado;
- BlobDetector.cpp: Usado para detecção e tratamento de Blobs/Elipses/*HoughCircles*;
- Util.cpp: Calibragem da câmera (Hardware);

_./_
- CoinCounter.cpp: Contém os elementos da classe a ser usada pelo main.cpp;
- main.cpp: Código principal.

No momento os valores são lidos apenas em Reais. A interface para o usuário é extremamente minimalista. Pretendemos implementar:

- [ ] Aumentar a flexibilidade do programa, inclusive para outros e mais tipos de moeda;
- [ ] Detectar outros nuances para diferenciação de moedas (cor, detalhes, grossura);
- [ ] Adicionar elementos na interface para deixar o programa mais amigável ao usuário.

O desempenho do programa fica melhor quando as moedas estão sobre uma superfície fosca e razoavelmente iluminada. O plano da câmera deve estar o mais paralelo o possível com o plano do background. No momento o programa está muito sensível a luz!

-----------------------
OBS:: Se você não estiver conseguindo dar make mesmo com todas as bibliotecas instaladas e atualizadas, uma das (ou ambas) coisas abaixo pode ajudar:

1. Em processamento-de-imgs/BlobDetector.hpp, tente trocar a linha 4 "#define labNote 0" para "#define labNote 1";
2. No Makefile, tente remover das LIBS (linha 4) a string "-isystem/usr/include".

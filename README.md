# Controle de Matriz de LEDs WS2812B (Neopixel) com Raspberry Pi Pico

/*  
-----------------------------
**Autor:** Matuzalem Guimaraes Leal  
**E-mail:** matuinfo@gmail.com  
**Data:** 2021-09-30  
**Curso:** EmbarcaTech 2025  
**Revisão:** v1.0  
-----------------------------
*/

## Descrição do Projeto

Este programa controla uma matriz de LEDs WS2812B (Neopixel) conectada ao Raspberry Pi Pico, exibindo números de 0 a 9 na matriz 5x5. Cada número é representado com uma cor diferente para facilitar sua identificação. O programa utiliza dois botões (A e B) para incrementar ou decrementar o número exibido na matriz. Além disso, um LED vermelho piscante indica que o programa está em execução.

---

## Funcionalidades Implementadas

1. **Exibição de Números na Matriz WS2812B:**
   - Os números de 0 a 9 são exibidos na matriz 5x5.
   - Cada número possui uma cor específica associada.

2. **Controle via Botões:**
   - **Botão A (GPIO 5):** Incrementa o número exibido na matriz.
   - **Botão B (GPIO 6):** Decrementa o número exibido na matriz.
   - Implementação de debouncing via software para evitar múltiplas detecções de pressionamento.

3. **Indicação de Execução:**
   - O LED vermelho (parte do LED RGB conectado aos GPIOs 11, 12 e 13) pisca continuamente a uma taxa de 5 vezes por segundo, indicando que o programa está em execução.

---

## Componentes Utilizados

- **Matriz 5x5 de LEDs WS2812B:** Conectada ao GPIO 7.
- **LED RGB:** 
  - Vermelho: GPIO 13
  - Verde: GPIO 11
  - Azul: GPIO 12
- **Botão A:** Conectado ao GPIO 5.
- **Botão B:** Conectado ao GPIO 6.
- **Resistores de Pull-Up Internos:** Utilizados nos botões A e B.

---

## Requisitos do Projeto

### 1. Uso de Interrupções:
- Todas as funcionalidades relacionadas aos botões (incremento e decremento) são implementadas utilizando rotinas de interrupção (IRQ).

### 2. Debouncing:
- O tratamento do bouncing dos botões é realizado via software para garantir que cada pressionamento seja detectado corretamente.

### 3. Controle de LEDs:
- O projeto demonstra o controle de LEDs comuns (LED RGB) e LEDs endereçáveis (WS2812B), mostrando diferentes técnicas de manipulação.

### 4. Organização do Código:
- O código está bem estruturado e comentado, facilitando a compreensão das funcionalidades implementadas.

---

## Entrega do Projeto

### 1. Código-Fonte:
- O código-fonte deve ser entregue em um arquivo ou repositório, contendo todos os arquivos necessários para sua execução.
- **Observação:** O código será avaliado pelo mentor. Caso não seja possível compilar e executar o projeto, a nota atribuída poderá ser nula.

### 2. Vídeo de Demonstração:
- Um vídeo de até 2 minutos deve ser produzido, no qual o aluno:
  - Aparece pessoalmente demonstrando o funcionamento do projeto.
  - Explica brevemente as funcionalidades implementadas.
  - Mostra o projeto em execução na placa BitDogLab.

---

## Observações

Este programa foi desenvolvido como parte do curso **EmbarcaTech 2025**, com o objetivo de consolidar os conceitos relacionados ao uso de interrupções no microcontrolador RP2040 e explorar as funcionalidades da placa BitDogLab.

Se você tiver dúvidas ou sugestões, sinta-se à vontade para entrar em contato através do e-mail: **matuinfo@gmail.com**.
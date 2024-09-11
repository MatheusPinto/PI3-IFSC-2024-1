```mermaid
---
title: TTITLE
---
stateDiagram-v2
%, text-decoration-line:underline, text-decoration-style:double,text-underline-position:under
    classDef center text-align:center, ::first-line{font-weight:bold, font-size:large};
    %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
    IDLE:::center:IDLE\n- PCINT enable
    DELAY_PRESSED:::center:DELAY_PRESSED\n- PCINT disable\n- set 80ms timer
    OK_PRESSED:::center:OK
    DOWN_PRESSED:::center:DOWN\n- set 2s timer
    UP_PRESSED:::center:UP\n- set 2s timer
    DOWN_BURST:::center:DOWN_BURST\n- set 1s timer
    UP_BURST:::center:UP_BURST\n- set 1s timer
    DELAY_RELEASED:::center:DELAY_RELEASED\n- set 80ms timer

    state chose_bt <<choice>>

    [*] --> IDLE
    IDLE --> DELAY_PRESSED : PCINT
    DELAY_RELEASED --> IDLE : after 80ms
    DELAY_PRESSED --> chose_bt : after 80ms
    chose_bt --> OK_PRESSED : OK = 0
    chose_bt --> DOWN_PRESSED : DOWN = 0
    chose_bt --> UP_PRESSED : UP = 0
    chose_bt --> DELAY_RELEASED : else
    OK_PRESSED --> DELAY_RELEASED : OK = 1
    DOWN_PRESSED --> DELAY_RELEASED : DOWN = 1
    UP_PRESSED --> DELAY_RELEASED : UP = 1
    DOWN_PRESSED --> DOWN_BURST : after 2s
    DOWN_BURST --> DOWN_BURST : after 1s
    DOWN_BURST --> DELAY_RELEASED : DOWN = 1
    UP_PRESSED --> UP_BURST : after 2s
    UP_BURST --> UP_BURST : after 1s
    UP_BURST --> DELAY_RELEASED : UP = 1


```
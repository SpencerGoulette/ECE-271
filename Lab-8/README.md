ECE 271 Microcomputer Architecture and Applications
University of Maine
Spring 2018     

Name
-----------
```
Spencer Goulette
```

Summary of Lab Status
-------
- [X] Pre-lab Assignment (2 points) 
- [ ] Basic in-lab assignments (15 points) 
- [ ] Something cool (3 points): 

What is your something cool?
-------

Pre-lab Questions
-------
1. PSC = 15
2. Time = Tcounter(CCRnew - CCRlast); Difference = 0.5s*1MHz = 500,000
3. toverflow = ARR/Tcounter = 65535/1MHz = 65.5ms
4. Check the UIF flag then reset

1. time = 1/tcounter = 1us
2. 150us = 1/MHz = 150, 38ms = 1/1MHz; difference = 38,000
3. ARR/mintime = 65535/1us = 0.65535s

TIMx_CR1 CEN = 1; TIMx_DIER CC1DE = 1, CC1E = 1; TIMx_CCMR1 IC2 PSC = 00, IC1F = 0000, CC1S = 01; TIMx_CCER CC1NP = 1, CC1P = 1, CC1E = 1;

Post-lab Questions
-------

Lab Credits
-------
* Did you received any help from someone other than Prof. Zhu and lab teaching assistants?

Suggestions and Comments
-------
* Report typos, errors, or suggestions into the "typos" session on piazza site (https://piazza.com/maine/spring2018/ece271)
* I offer one dollar each new typo or error in the 3rd edition.

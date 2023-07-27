# DN-IEEE802.11DCF-TP

As a Computer assignment for Data Networks course an analysis on IEEE 802.11 DCF mode was done. 

The work started by examining the Markov model presented in `Performance analysis of the IEEE 802.11 distributed coordination function`
by G.Bianchi which can be accesed at  `https://ieeexplore.ieee.org/document/840210`.

The assignment required us to model the CSMA/CA mode, and derive the **throughput** using 3 different approaches.

**1-** First method is done by coding an analytical solution for a part of the computer assignment which uses the equations derived in the paper.
A code using C++ and also a simpler one which uses MATLAB functions is presented.

**2-** Second method, which was the main contribution and challenge required me to study the CSMA/CA mode in detail and using the specific variables,
simulate the channel contention between different users, which was done in C++.

**3-** Last method used ns-3 simulator and without getting into details of protocol, got us familiar with tools like `WAF` and report generation.

Different approaches and the differences was reported as the assignment results.

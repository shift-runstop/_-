# _ランナー
\ i ma_eht 1nvers1on
## $F(ω)$

Precise frequency analysis and manipulation of grains
Easy implementation of pitch-shifting and time-stretching
Spectral processing techniques like cross-synthesis between grains
continuous Fourier transform $F(ω)$ of a function $f(t)$ is:
$$
F(ω) = \int_{-∞}^{∞} f(t) e^{-iωt} dt
$$
The inverse transform is:
$$
f(t) = \frac{1}{2π} \int_{-∞}^{∞} F(ω) e^{iωt} dω
$$
(DFT) is:
$$
X_k = \sum_{n=0}^{N-1} x_n e^{-i2πkn/N}
$$
(IDFT) is:
$$
x_n = \frac{1}{N} \sum_{k=0}^{N-1} X_k e^{i2πkn/N}
$$
$N$ = num_samples
QuaternionsOC[200~experimental forms of granular synthesis perhaps in
spatializing grains in a 3D audio environment or in creating novel ways of
transforming grain parameters
eof

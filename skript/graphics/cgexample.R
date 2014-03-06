d <- read.csv("cgresults.csv")

d

pdf("cgresults.pdf", 8, 6)

k = seq(1,20,1)

plot(0, 0, type = "n", log = "y",
	xlim = c(1,20), ylim = c(1e-7,1),
	xlab = "Iteration", ylab = "Fehler",
	main = "Fehler des CG Verfahrens")

points(d[d$j == 2,]$k, d[d$j == 2,]$e, pch = 1)
points(d[d$j == 4,]$k, d[d$j == 4,]$e, pch = 2)
points(d[d$j == 6,]$k, d[d$j == 6,]$e, pch = 3)
points(d[d$j == 8,]$k, d[d$j == 8,]$e, pch = 4)

i = 2
q = d[(d$j == i) & (d$k == 1),]$q
e = d[(d$j == i) & (d$k == 2),]$e
lines(k, e * q^(k-2))

i = 4
q = d[(d$j == i) & (d$k == 1),]$q
e = d[(d$j == i) & (d$k == 2),]$e
lines(k, e * q^(k-2))

i = 6
q = d[(d$j == i) & (d$k == 1),]$q
e = d[(d$j == i) & (d$k == 2),]$e
lines(k, e * q^(k-2))

i = 8
q = d[(d$j == i) & (d$k == 1),]$q
e = d[(d$j == i) & (d$k == 2),]$e
lines(k, e * q^(k-2))


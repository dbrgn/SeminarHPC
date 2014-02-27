d <- read.csv("results.csv")

pdf("results-heat.pdf", 8, 6)
plot(d$n, d$time, log = "xy",
	main = "Laufzeit Lösung der Wärmeleitungsgleichung",
	xlab = "n", ylab = "Laufzeit [s]")

#!/usr/bin/env Rscript
library(ggplot2)
library(reshape)

frame2vec = function (data = NA) {
    m = as.matrix(data)
    q = c()
    for (i in seq(1:nrow(m))){
          q = c(q, m[i,])
          names(q) = NULL
    }
    cond = q < -8
    q[cond]=-8
    return(q)
}

fn_pdsi = 'monthly/original/PDSI.tbl'
cal_pdsi = 'monthly/self_cal/PDSI.tbl'

pdsi = read.table(fn_pdsi)
cpdsi = read.table(cal_pdsi)
pdsi = frame2vec(pdsi[, 2:13])
cpdsi = frame2vec(cpdsi[, 2:13])
ts1 <- ts(pdsi, start=c(1893,1), end=c(2003, 12), frequency=12) 
df = data.frame(time=as.numeric(time(ts1)), pdsi=pdsi, cal=cpdsi)
meltdf = melt(df, id=c("time"))
bp = ggplot(meltdf,aes(x=time,y=value,colour=variable,group=variable)) + geom_line()
ggsave("ggplot1.pdf")

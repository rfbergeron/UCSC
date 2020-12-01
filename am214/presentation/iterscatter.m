function iterscatter(y)
    xs = y(1:end-1);
    ys = y(2:end);
    scatter(xs,ys);
end
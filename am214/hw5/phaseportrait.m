function phaseportrait(func, tspan, xspan, yspan, npoints, fig)
    trange = [tspan(1), tspan(end)];
    xmax = xspan(end);
    xmin = xspan(1);
    ymax = xspan(end);
    ymin = xspan(1);
    ts = [];
    ys = [];
    odeopts = odeset('MaxStep', 1e-2, 'InitialStep', 1e-3, 'RelTol', 1e-2, 'AbsTol', 1e-2);

    % This defines and labels the axes of the phase portrait
    figure(fig);
    axis([xmin xmax ymin ymax]);
        xlabel('x','FontSize',20);
        ylabel('y','FontSize',20);
    hold on;

    for i=0:npoints
        x0 = xmin + i*(xmax-xmin)/npoints;
        for j=0:npoints
            y0 = ymin + j*(ymax-ymin)/npoints;
            [t,y] = ode45(func,trange,[x0;y0],odeopts);
            plot(y(:,1),y(:,2));
        end
    end
end

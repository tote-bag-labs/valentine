/*
  ==============================================================================

    LagrangeInterp.h
    Created: 23 Jan 2020 9:03:40am
    Author:  Jose Diaz

  ==============================================================================
*/

#pragma once

/**
From Pirkle's FXObjects.h
@doLagrangeInterpolation
\ingroup FX-Functions

@brief implements n-order Lagrange Interpolation

\param x - Pointer to an array containing the x-coordinates of the input values
\param y - Pointer to an array containing the y-coordinates of the input values
\param n - the order of the interpolator, this is also the length of the x,y input arrays
\param xbar - The x-coorinates whose y-value we want to interpolate
\return the interpolated value
*/
inline double doLagrangeInterpolation (double* x, double* y, int n, double xbar)
{
    int i, j;
    double fx = 0.0;
    double l = 1.0;
    for (i = 0; i < n; i++)
    {
        l = 1.0;
        for (j = 0; j < n; j++)
        {
            if (j != i)
                l *= (xbar - x[j]) / (x[i] - x[j]);
        }
        fx += l * y[i];
    }
    return (fx);
}




/*
	finding the root of penalized quartic function for beta_ij
*/
float quartic_roots(float a, float b, float, c, float d, float e)
{
	float DELTA, P, R, DELTA_0, D;

	DELTA = 256 * pow(a*e, 3) - 192 * b*d*pow(a*e, 2) - 128 * pow(a*c*e, 2) + 144 * c*e*pow(a*d, 2) - 27 * pow(a, 2)*pow(d, 4)
		+ 144 * a*c*pow(b*e, 2) - 6 * a*e*pow(b*d, 2) - 80 * a*b*d*e*pow(c, 2) + 18 * a*b*c*pow(d, 3) + 16 * a*e*pow(c, 4)
		- 4 * a*pow(c, 3)*pow(d, 2) - 27 * pow(b, 4)*pow(e, 2) + 18 * pow(b, 3)*c*d*e - 4 * pow(b*d, 3) - 4 * pow(b, 2)*pow(c, 3)*e +
		pow(b*c*d, 2);

	P = 8 * a * c - 3 * pow(b, 2);
	R = pow(b, 3) + 8 * d*pow(a, 2) - 4 * a*b*c;
	DELTA_0 = pow(c, 2) - 3 * b * d + 12 * a * e;
	D = 64 * pow(a, 3)*e - 16 * pow(a*c, 2) + 16 * a*c*pow(b, 2) - 16 * pow(a, 2)*b*d - 3 * pow(b, 4);

	if (DELTA > 0 && P > 0 && D > 0)
		printf("No real roots \n");

	p = (8 * a*c - 3 * pow(b, 2)) / (8 * pow(a, 2));
	q = (pow(b, 3) - 4 * a*b*c + 8 * pow(a, 2)*d) / (8 * pow(a, 3));
	DELTA_1 = 2 * pow(c, 3) - 9 * b*c*d + 27 * pow(b, 2)*e + 27 * a*pow(d, 2) - 72 * a*c*e;
	Q = cbrt((DELTA_1)+sqrt(pow(DELTA_1, 2) - 4 * pow(DELTA_0, 3)) / 2);
	S = sqrt(-2 * p / 3 + (Q + DELTA_0 / Q) / (3 * a)) / 2;
	x_1 = -b / (4 * a) - S + sqrt(-4 * pow(S, 2) - 2 * p + q / S) / 2;
	x_2 = -b / (4 * a) - S - sqrt(-4 * pow(S, 2) - 2 * p + q / S) / 2;
	x_3 = -b / (4 * a) + S + sqrt(-4 * pow(S, 2) - 2 * p - q / S) / 2;
	x_4 = -b / (4 * a) + S - sqrt(-4 * pow(S, 2) - 2 * p - q / S) / 2;
}
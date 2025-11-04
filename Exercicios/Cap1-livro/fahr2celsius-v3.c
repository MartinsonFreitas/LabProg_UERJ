/*
 * fahr2celsius.c
 * 
 * Copyright 2025 Martinson <martinson@martinson-VPCEB17FB>
 * 
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 * 
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 * 
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston,
 * MA 02110-1301, USA.
 * 
 * 
 */


#include <stdio.h>

/* imprime tabela Fahrenheit-Celsius */
int main(void)
	{
		int fahr = 0;
		
		for (fahr = 0; fahr <= 300; fahr = fahr +20){
			printf("%3d %6.1f\n", fahr, (5.0/9.0) * (fahr - 32));
		}
		
	return 0;
}


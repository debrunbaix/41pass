#include "../include/pwman.h"

/**
 * Chiffrement/déchiffrement simple par XOR avec rotation
 * Très basique mais suffisant pour le projet
 */
void	simple_encrypt(char *data, int len)
{
		int i;
		char key = CIPHER_KEY;

		for (i = 0; i < len && data[i] != '\0'; i++)
		{
				// XOR avec la clé + rotation basée sur la position
				data[i] = data[i] ^ (key + (i % 8));
				// Rotation de la clé pour plus de variation
				key = (key << 1) | (key >> 7);
		}
}

/**
 * Le déchiffrement est identique au chiffrement avec XOR
 */
void	simple_decrypt(char *data, int len)
{
		simple_encrypt(data, len);  // XOR est réversible
}

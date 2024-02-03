#include <stdio.h>
#include <stdlib.h>

int	main()
{
	char str[] = "Rnpu cebwrpg bs gur 97 Pbzzba Pber pbagnvaf na rapbqrq uvag. Sbe rnpu pvepyr, bayl bar cebwrpg cebivqrf gur pbeerpg uvag arrqrq sbe gur arkg pvepyr. Guvf punyyratr vf vaqvivqhny, gurer vf bayl n cevmr sbe bar fghqrag jvaare cebivqvat nyy qrpbqrq zrffntrf. Nal nqinagntrq crbcyr pna cynl, yvxr pheerag be sbezre fgnss, ohg gur cevmr jvyy erznva flzobyvp. Gur uvag sbe guvf svefg cebwrpg vf: Ynetr pbjf trarebfvgl pbzrf jvgu punegf naq sbhe oybaqr ungf gb qrsl hccre tenivgl ureb";
	for (int i = 0; i < strlen(str); i++)
	{
		if (str[i] >= 'A' && str[i] <= 'Z')
			str[i] += 32;
		if (str[i] >= 'a' && str[i] <= 'z')
		{
			if (str[i] != ' ' && str[i] - 13 >= 'a')
				printf("%c", str[i] - 13);
			else if (str[i] != ' ')
				printf("%c", str[i] + 13);
		}
		else
			printf("%c", str[i]);
	}
}
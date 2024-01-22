#include "../../includes/webserv.hpp"

seNode	*newseNode()
{
	seNode *node = new seNode;
	node->elem_n = 0;
	bzero(node->elem, sizeof(node->elem));
	node->next = NULL;
	return (node);
}

void	seLstPushBack(seLst &lst, seNode *newNode)
{
	if (!lst.head || !newNode)
		return ;
	
	lst.tail->next = newNode;
	lst.tail = newNode;
	lst.size++;

}

void	seLstFree(seLst &lst)
{
	seNode *node = lst.head;
	seNode *aux;

	while (node)
	{
		aux = node;
		node = node->next;
		aux->next = NULL;
		free(aux);
	}
}

char	*seLstToStr(seLst &lst)
{
	char	*str = (char *)malloc(lst.bytes + 1);
	seNode *node = lst.head;
	int	i = 0;
	while (node)
	{
		int	len = node->elem_n;
		for (int j = 0; j < len; i++, j++)
			str[i] = node->elem[j];
		node = node->next;
	}
	str[i] = '\0';
	//std::cout << "----------Hizo bien transformación de lista a char*------------" << std::endl;
	//std::cout << str << std::endl;
	return (str);
}

size_t	countCharinStr(char *str, char c)
{
	size_t	count = 0;
	
	for (size_t i = 0; i < strlen(str); i++)
		if	(str[i] == c)
			count++;
	return (count);
}

char	*readFileSeLst(int fd)
{
	seLst	lst = {newseNode(), lst.head, 0, 0};
	//int		lines = 0;
	seNode 	*node = lst.head;
	ssize_t	readBytes = read(fd, &node->elem, MAX_READ);
	node->elem_n = readBytes;
	lst.bytes = readBytes;
	while (readBytes > 0)
	{
		if (node->elem_n + MAX_READ >= MAX_SE_ELEM)
		{
			node->elem[node->elem_n] = '\0';
			seLstPushBack(lst, newseNode());
			node = lst.tail;
		}
		if (readBytes < 0)
		{
			seLstFree(lst);
			return (NULL);
		}
		//lines += countCharinStr(&node->elem[node->elem_n], '\n');
		readBytes = read(fd, &node->elem[node->elem_n], MAX_READ);
		node->elem_n += readBytes;
		lst.bytes += readBytes;
	}
	node->elem[node->elem_n] = '\0';
	char	*fileArr = seLstToStr(lst);
	seLstFree(lst);
	return (fileArr);
}

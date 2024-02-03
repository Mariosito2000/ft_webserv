#include "../../includes/webserv.hpp"

size_t	countCharinStr(const char *str, char c)
{
	int	count = 0;
	
	for (size_t i = 0; i < strlen(str); i++)
		if	(str[i] == c)
			count++;
	return (count);
}

int	cmpLocations(parseTree *loc, parseTree *cmp)
{
	std::string dir = loc->context._args[0];
	std::string	cmp_cast = cmp->context._args[0];
	
	int	slash1 = countCharinStr((dir).c_str(), '/');
	int	slash2 = countCharinStr((cmp_cast).c_str(), '/');
	if (slash1 > slash2)
		return (slash1 - slash2);
	if (slash1 < slash2)
		return (slash1 - slash2);
	if (dir == "/")
		return (1);

	std::string	s1;
	std::string	s2;
	for (size_t start1 = 1, start2 = 1, i = 1, j = 1; start1 < dir.length() && start2 < cmp_cast.length(); start1++, start2++)
	{
		for ( ; dir[start1] && dir[start1] != '/'; start1++);
		for ( ; cmp_cast[start2] && cmp_cast[start2] != '/'; start2++);
		s1 = dir.substr(i, start1 - i);
		s2 = cmp_cast.substr(j, start2 - j);
		if (s1 != s2)
			return (0);
		i = start1 + 1;
		j = start2 + 1;
	}
	return (1);
}

void	linearInsertLoc(std::vector<parseTree *> locs, parseTree *add)
{
	typedef std::vector<parseTree *>::iterator	itPt;
	itPt b = locs.begin();

	for (itPt e = locs.end(); b != e; b++)
	{
		if (cmpLocations(*b, add) > 0)
			break ;
	}
	locs.insert(b, add);
}

/*typedef struct s_ranges{
	std::vector<int_tuple>	ranges;
	typedef enum states {EMPTY, PARTIAL, COMPLETE} states;
	std::vector<short>		state;
	size_t					n;
}	t_ranges;

void	setRangesDel(std::string &s, char del, t_ranges &ranges)
{
	int_tuple	range;
	short		state;

	ranges.n = 0;
	for (size_t i = 0; i < s.size();)
	{
		range.i = i;
		range.e = s.find(del, i);
		if (range.i == range.e)
			state = t_ranges::EMPTY;
		else if ((size_t)range.e == s.npos)
		{
			range.e = s.size() - 1;
			state = t_ranges::PARTIAL;
		}
		else
		{
			range.e--;
			state = t_ranges::COMPLETE;
		}
		ranges.ranges.push_back(range);
		ranges.state.push_back(state);
		ranges.n++;
		i = range.e + 1;
		for (; i < s.size() && s[i] == del; i++);
	}
}

size_t	countCompletedRanges(t_ranges &r)
{
	size_t n = 0;
	for (size_t i = 0; i < r.n; i++)
	{
		if (r.state[i] == r.COMPLETE)
			n++;
	}
	return (n);
}

int	cmp_str_ranges(const char *s1, size_t n1, const char *s2, size_t n2)
{
	if (!s1 || !s2)
		return (-1);
	if (n1 != n2)
		return (-2);
	int	dif;
	for (size_t i = 0 ; i < n1; i++)
	{
		//std::cout << "Comparar: " << s1[i] << " y " << s2[i] << std::endl;
		dif = s1[i] - s2[i];
		if (dif != 0)
			return (dif);
	}
	return (0);
}

int	cmpLocs(std::string &loc1, std::string &loc2)
{
	t_ranges	r1;
	t_ranges	r2;

	setRangesDel(loc1, '/', r1);
	setRangesDel(loc2, '/', r2);
	size_t dif = countCompletedRanges(r1) - countCompletedRanges(r2);
	if (dif != 0)
		return (dif);
	for (size_t	i = 0; i < r1.n; i++)
	{
		if (r1.state[i] == t_ranges::COMPLETE)
		{
			size_t	n1 = (r1.ranges[i].e - r1.ranges[i].i);
			size_t	n2 = (r2.ranges[i].e - r2.ranges[i].i);
			dif = n1 - n2;
			if (dif != 0)
				return (dif);
			dif =  cmp_str_ranges(loc1.c_str(), n1, loc2.c_str(), n2);
			if (dif != 0)
				return (dif);
		}
	}
	return (0);
}

void	printRanges(t_ranges &r, std::string &s)
{
	std::cout << "-------------PRINT RANGES-------------" << std::endl;
	for (size_t i = 0; i < r.ranges.size(); i++)
	{
		std::cout << "RANGE " << i << ": [" << r.ranges[i].i << ", " << r.ranges[i].e
					<< "] | STATE: " << r.state[i] << std::endl;
		std::cout << "SUBSTR: ";
		for (int j = r.ranges[i].i; j <= r.ranges[i].e; j++)
			std::cout << s[j];
		std::cout << std::endl;
	}
}

int	main(void)
{
	std::string	a("/pepe/juan/manolo");
	t_ranges r1;
	setRangesDel(a, '/', r1);
	printRanges(r1, a);

	std::string	b("/pepe/jose");
	t_ranges r2;
	setRangesDel(b, '/', r2);
	printRanges(r2, b);
	
}*/

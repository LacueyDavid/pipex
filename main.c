/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dlacuey <dlacuey@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/08/04 19:47:30 by dlacuey           #+#    #+#             */
/*   Updated: 2023/08/08 17:32:56 by dlacuey          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "libft/libft.h"
#include "pipex.h"

#include <fcntl.h>
#include <stdio.h>
#include <sys/wait.h>

static void	clean_child(t_pipex pipex)
{
	close(pipex.infile_fd);
	close(pipex.outfile_fd);
	close(pipex.pipefds[0]);
	close(pipex.pipefds[1]);
	free_strs(pipex.paths);
}

static void	first_child(t_pipex pipex, char **argv, char **envp)
{
	char	**args;
	char	*command;

	if (dup2(pipex.pipefds[1], STDOUT_FILENO) == -1)
		(perror("Error, dup2 pipefds[1] failed"), clean_child(pipex), exit(1));
	close(pipex.pipefds[0]);
	if (dup2(pipex.infile_fd, STDIN_FILENO) == -1)
		(perror("Error, dup2 infile_fd failed"), clean_child(pipex), exit(1));
	args = ft_split(argv[2], ' ');
	if (args == NULL)
		(perror("Error, args not found"), clean_child(pipex), exit(1));
	command = get_command(args[0], pipex.paths);
	if (command == NULL)
		(perror("Error, command not found"), clean_child(pipex),
			free_strs(args), exit(1));
	execve(command, args, envp);
	(perror("Error, execve failed"), clean_child(pipex), exit(1));
}

static void	second_child(t_pipex pipex, char **argv, char **envp)
{
	char	**args;
	char	*command;

	if (dup2(pipex.pipefds[0], STDIN_FILENO) == -1)
		(perror("Error, dup2 pipefds[0] failed"), clean_child(pipex), exit(1));
	close(pipex.pipefds[1]);
	if (dup2(pipex.outfile_fd, STDOUT_FILENO) == -1)
		(perror("Error, dup2 outfile_fd failed"), clean_child(pipex), exit(1));
	args = ft_split(argv[3], ' ');
	if (args == NULL)
		(perror("Error, args not found"), clean_child(pipex), exit(1));
	command = get_command(args[0], pipex.paths);
	if (command == NULL)
		(perror("Error, command not found"), clean_child(pipex),
			free_strs(args), exit(1));
	execve(command, args, envp);
	(perror("Error, execve failed"), clean_child(pipex), exit(1));
}

static int	creat_processes(t_pipex pipex, char **argv, char **envp)
{
	pid_t	pid1;
	pid_t	pid2;

	pid1 = fork();
	if (pid1 == 0)
		first_child(pipex, argv, envp);
	else if (pid1 == -1)
		return (perror("Error, fork child 1 failed"), 1);
	pid2 = fork();
	if (pid2 == 0)
		second_child(pipex, argv, envp);
	else if (pid2 == -1)
		return (perror("Error, fork child 2 failed"), 1);
	close(pipex.pipefds[0]);
	close(pipex.pipefds[1]);
	waitpid(pid1, NULL, 0);
	waitpid(pid2, NULL, 0);
	return (0);
}

int	main(int argc, char **argv, char **envp)
{
	t_pipex		pipex;
	int			exit;

	if (argc != 5)
		return (print_str("Error. Number of arguments should be five\n"), 1);
	pipex.infile_fd = open(argv[1], O_RDONLY);
	if (pipex.infile_fd == -1)
		return (perror("Error, infile not opened"), 1);
	pipex.outfile_fd = open(argv[argc - 1], O_WRONLY | O_CREAT | O_TRUNC, 0644);
	if (pipex.outfile_fd == -1)
		return (perror("Error, outfile not opened"), close(pipex.infile_fd), 1);
	if (pipe(pipex.pipefds) == -1)
		return (perror("Error, pipe not opened"), close(pipex.infile_fd),
			close(pipex.outfile_fd), 1);
	pipex.paths = find_paths(envp);
	if (pipex.paths == NULL)
		return (perror("Error, paths not found"), close(pipex.infile_fd),
			close(pipex.outfile_fd), close(pipex.pipefds[0]),
			close(pipex.pipefds[1]), 1);
	exit = creat_processes(pipex, argv, envp);
	(free_strs(pipex.paths), close(pipex.infile_fd), close(pipex.outfile_fd),
		close(pipex.pipefds[0]), close(pipex.pipefds[1]));
	return (exit);
}

<?php
define('BASE_DIR', dirname(__DIR__));

$indent = 3;

$conf = array(
	array(
		'role' => 'doc',
		'pattern' => 'CREDITS',
	),
	array(
		'role' => 'doc',
		'pattern' => 'LICENSE',
	),
	array(
		'role' => 'doc',
		'pattern' => '*.md',
	),
	array(
		'role' => 'src',
		'pattern' => 'config.m4',
	),
	array(
		'role' => 'src',
		'pattern' => 'config.w32',
	),
	array(
		'role' => 'src',
		'pattern' => 'Makefile.frag',
	),
	array(
		'role' => 'src',
		'pattern' => 'php_jsond*.h',
	),
	array(
		'role' => 'src',
		'pattern' => 'jsond*.h',
	),
	array(
		'role' => 'src',
		'pattern' => 'jsond*.c',
	),
	array(
		'role' => 'src',
		'pattern' => 'jsond*.y',
	),
	array(
		'role' => 'src',
		'pattern' => 'jsond*.re',
	),
	array(
		'dir' => 'tests',
		'role' => 'test',
		'pattern' => '*.phpt',
	),
);

function print_files($indent_str, $role, $pattern, $rel_dir = "") {
	if ($rel_dir)
		$rel_dir .= "/";
	$abs_dir = BASE_DIR . "/" . $rel_dir;
	foreach (glob($abs_dir . $pattern) as $path) {
		printf('%s<file role="%s" name="%s"/>%s', $indent_str, $role, substr($path, strlen($abs_dir)), "\n");
	}
}

$indent_str = str_repeat(' ', $indent);
foreach ($conf as $e) {
	if (!isset($e['dir']))
		$e['dir'] = '';
	if ($e['dir']) {
		printf('%s<dir name="%s">%s', $indent_str, $e['dir'], "\n");
		$indent_str .= ' ';
	}
	print_files($indent_str, $e['role'], $e['pattern'], $e['dir']);
	if ($e['dir']) {
		$indent_str = str_repeat(' ', $indent);
		printf('%s</dir>%s', $indent_str, "\n");
	}
}


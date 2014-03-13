<?php

$json = '{"i": 23, "array": [1, null, false, true, ["aha", "baba", 23, {"test": 23}]]}';

$iters = isset($argv[1]) ? $argv[1] : 100000;

function test_json($json) {
	global $iters;
	
	$start = microtime(true);
	for ($i = 0; $i < $iters; $i++) {
		json_decode($json);
	}
	$end = microtime(true);

	$duration = $end - $start;
	printf("JSON:  time for %d iterations: %f" . PHP_EOL, $iters, $duration);
}


function test_jsond($json) {
	global $iters;
	
	$start = microtime(true);
	for ($i = 0; $i < $iters; $i++) {
		jsond_decode($json);
	}
	$end = microtime(true);

	$duration = $end - $start;
	printf("JSOND: time for %d iterations: %f" . PHP_EOL, $iters, $duration);
}


$jsons = array(
	'{"i": 23, "array": [1, null, false, true, ["aha", "baba", 23, {"test": 23}]]}',
	'[1, 2, 3, 4, 5, 6, 7, 8, 9, 10]',
	'{"a": 23.2234232}',
	'{"long-str": "At vero eos et accusamus et iusto odio dignissimos ducimus qui blanditiis praesentium voluptatum deleniti atque corrupti quos dolores et quas molestias excepturi sint occaecati cupiditate non provident, similique sunt in culpa qui officia deserunt mollitia animi, id est laborum et dolorum fuga. Et harum quidem rerum facilis est et expedita distinctio. Nam libero tempore, cum soluta nobis est eligendi optio cumque nihil impedit quo minus id quod maxime placeat facere possimus, omnis voluptas assumenda est, omnis dolor repellendus. Temporibus autem quibusdam et aut officiis debitis aut rerum necessitatibus saepe eveniet ut et voluptates repudiandae sint et molestiae non recusandae. Itaque earum rerum hic tenetur a sapiente delectus, ut aut reiciendis voluptatibus maiores alias consequatur aut perferendis doloribus asperiores repellat.", }',
	json_encode($_SERVER),
);

foreach ($jsons as $json) {
	echo "STR: $json\n";
	test_json($json);
	test_jsond($json);
}

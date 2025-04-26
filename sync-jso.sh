#!/bin/bash

if readlink "${BASH_SOURCE[0]}" > /dev/null; then
  jsond_base_dir="$( dirname "$( readlink "${BASH_SOURCE[0]}" )" )"
else  
  jsond_base_dir="$( cd -P "$( dirname "${BASH_SOURCE[0]}" )" && pwd )"
fi

jso_branch="${1:-master}"
jso_action="${2:-all}"
jso_src_dir="$jsond_base_dir/jso-$jso_branch"
jso_dest_dir="$jsond_base_dir/jso"
jso_archive="$jso_branch.zip"
jso_download_url="https://github.com/bukka/jso/archive/refs/heads/$jso_archive"

core_files=(
  jsond.c
  jsond_buffer.c
  jsond_dtoa.c
  jsond_encoder.c
  jsond_parser.tab.c
  jsond_scanner.c
)

# install
jso_install() {
  rm -rf "$jso_src_dir"

  wget "$jso_download_url"
  unzip "$jso_archive"
  rm "$jso_archive"

  cd "$jso_src_dir"
  ./autogen.sh
  ./configure
  make
  cd -

  rm -rf "$jso_dest_dir"
  mkdir "$jso_dest_dir"
}

# clean up
jso_cleanup() {
  rm -rf "$jso_src_dir" "$jsond_base_dir/ar-lib" "$jsond_base_dir/compile" "$jsond_base_dir/depcomp" "$jsond_base_dir/test-driver"
}

# collect C and header files
jso_collect_files() {
  cd "$jso_src_dir"

  mapfile -t jso_c_files < <(find src -type f -name 'jso*.c' | sed 's|^src/||' | sort)
  mapfile -t jso_h_files < <(find src -type f -name 'jso*.h' | sed 's|^src/||' | sort)

  for f in "${jso_c_files[@]}" "${jso_h_files[@]}"; do
    mkdir -p "$jso_dest_dir/$(dirname "$f")"
    cp "src/$f" "$jso_dest_dir/$f"
  done
}

jso_update_config_m4() {
  # Remove old PHP_JSO_SOURCES block
  sed -i.bak '/^  PHP_JSO_SOURCES=m4_normalize(\["/,/^  "])$/d' "$jsond_base_dir/config.m4"

  # Create updated source block
  src_block=$(mktemp)
  {
    echo '  PHP_JSO_SOURCES=m4_normalize(["'
    for f in "${jso_c_files[@]}"; do
      echo "    jso/${f}"
    done
    echo '  "])'
  } > "$src_block"

  # Insert updated block after AC_DEFINE([HAVE_JSOND]...)
  awk -v src_block="$src_block" '
    /AC_DEFINE\(\[HAVE_JSOND\]/ {
      print
      while ((getline line < src_block) > 0) print line
      close(src_block)
      next
    }
    { print }
  ' "$jsond_base_dir/config.m4" > "$jsond_base_dir/config.m4.tmp" && mv "$jsond_base_dir/config.m4.tmp" "$jsond_base_dir/config.m4"

  rm -f "$src_block" "$jsond_base_dir/config.m4.bak"
}

jso_update_config_w32() {
  # delete existing EXTENSION("jsond", ...) block inside the if (PHP_JSOND != "no")
  sed -i.bak '/^	EXTENSION("jsond"/,/^	");/d' "$jsond_base_dir/config.w32"

  # build new EXTENSION block into a temp file
  ext_block=$(mktemp)
  {
    echo '	EXTENSION("jsond", "\\'
    printf '\t\t%s \\\n' "${core_files[@]}"
    printf '\t\tjso/%s \\\n' "${jso_c_files[@]/}"
    echo '	");'
  } > "$ext_block"

  # inject inside the if (PHP_JSOND != "no") block
  awk -v ext_block="$ext_block" '
    /^\s*if\s*\(PHP_JSOND\s*!=\s*"no"\)\s*\{/ {
      print
      while ((getline line < ext_block) > 0) print line
      close(ext_block)
      next
    }
    { print }
  ' "$jsond_base_dir/config.w32" > "$jsond_base_dir/config.w32.tmp" && mv "$jsond_base_dir/config.w32.tmp" "$jsond_base_dir/config.w32"

  rm -f "$ext_block" "$jsond_base_dir/config.w32.bak"
}


jso_update() {
  jso_collect_files
  jso_update_config_m4
  jso_update_config_w32
}

case "$jso_action" in
  install)
    jso_install
    ;;
  update)
    jso_update
    ;;
  cleanup)
    jso_cleanup
    ;;
  all)
    jso_install
    jso_update
    jso_cleanup
    ;;
  *)
    echo "Usage: $0 [branch] [install|update|cleanup|all]"
    exit 1
    ;;
esac

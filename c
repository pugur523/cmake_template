#/!bin/bash

set -e

script_dir=$(cd $(dirname $0) && pwd)
build_scripts_dir=${script_dir}/src/build/scripts
docker_dir=${build_scripts_dir}/../docker
debug_out_bin_dir=${script_dir}/out/build/linux/x86_64/debug/bin
release_out_bin_dir=${script_dir}/out/build/linux/x86_64/release/bin
project_name="sample_project"

action=$1

if [ -n "$action" ]; then
  shift 1
  remain_args=$@
else
  action="build"
  remain_args="--build_mode=all"
fi

if [ "$action" = "build" ]; then
  ${build_scripts_dir}/build.py $remain_args
elif [ "$action" = "run" ]; then
  ${debug_out_bin_dir}/${project_name} $remain_args
elif [ "$action" = "test" ]; then
  ${debug_out_bin_dir}/${project_name}_test $remain_args
elif [ "$action" = "bench" ]; then
  ${release_out_bin_dir}/${project_name}_bench $remain_args
elif [ "$action" = "docker" ]; then
  ${docker_dir}/build.sh $remain_args
elif [ "$action" = "pcheck" ]; then
  # check before push
  ${build_scripts_dir}/build.py --build_mode=all
  ${debug_out_bin_dir}/${project_name}_test
  echo "push check completed successfully"
else
  echo "unknown option specified; aborted"
  exit 1
fi

exit 0

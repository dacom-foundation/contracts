docker run --rm -it --name cdt_v4.1.0 \
  --volume $(pwd):/project \
  -w /project \
  dicoop/cdt_v4.1.0 /bin/bash

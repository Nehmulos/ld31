gimp -n -i -b - <<EOF
(let* ((file's (append (cadr (file-glob "gfxsrc/*.xcf" 1))
                       (cadr (file-glob "gfxsrc/editor/*.xcf" 1))))
       (filename "")
       (image 0)
       (layer 0))
  (while (pair? file's)
    (set! image (car (gimp-file-load RUN-NONINTERACTIVE (car file's) (car file's))))
    (set! layer (car (gimp-image-merge-visible-layers image CLIP-TO-IMAGE)))
    (set! filename (string-append (substring (car file's) 0 (- (string-length (car file's)) 4)) ".png"))
    (gimp-file-save RUN-NONINTERACTIVE image layer filename filename)
    (gimp-image-delete image)
    (set! file's (cdr file's)))
  (gimp-quit 0))
EOF
mkdir -p gfx/editor
mv gfxsrc/*.png gfx/
mv gfxsrc/editor/*.png gfx/editor

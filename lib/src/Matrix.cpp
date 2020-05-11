#include <assert.h>
#include <stdlib.h>
#include <stddef.h>
#include <stdarg.h>
#include <algorithm>

#include "Commons.h"
#include "Matrix.h"

/* Swap rows r1 and r2 of a matrix.
   This is one of the three "elementary row operations". */
static void swapRows(matrix_t *m, uint32_t r1, uint32_t r2);

/* Add a multiple of row r2 to row r1.
   Also known as a "shear" operation.
   This is one of the three "elementary row operations". */
static void shearRow(matrix_t *m, uint32_t r1,
                     uint32_t r2, double scalar);

/* Multiply row r of a matrix by a scalar.
   This is one of the three "elementary row operations". */
static void scaleRow(matrix_t *m, uint32_t r, double scalar);


//todo remove asserts. return null if something is wrong
//and release resources.
matrix_t *MatrixAlloc(uint32_t rows,
                      uint32_t cols) {
  assert(rows);
  assert(cols);
  matrix_t *mtx = new matrix_t;
  assert(mtx);
  mtx->rows = rows;
  mtx->cols = cols;
  mtx->data = new double*[rows];
  assert(mtx->data);

  for (uint32_t r = 0; r < rows; ++r) {
    mtx->data[r] = new double[cols];
    assert(mtx->data[r]);
    for (uint32_t c = 0; c < cols; ++c)
      mtx->data[r][c] = 0.0;
  }
  return mtx;
}
//////////////////////////////////////////////////////////////////////////

void MatrixFree(matrix_t *m) {
  assert(m);
  while (m->rows--)
    delete [] m->data[m->rows];
  delete [] m->data;
  delete m;
}
//////////////////////////////////////////////////////////////////////////

//todo change . let's use some double *args (for example).
void MatrixSet(matrix_t *m, ...) {
  assert(m);
  va_list ap;
  va_start(ap, m);
  for (uint32_t r = 0; r < m->rows; ++r) {
    for (uint32_t c = 0; c < m->cols; ++c) {
      m->data[r][c] = va_arg(ap, double);
    }
  }
  va_end(ap);
}
//////////////////////////////////////////////////////////////////////////

void MatrixSetIdentityDiag(matrix_t *m) {
  assert(m);
  for (uint32_t r = 0; r < m->rows; ++r) {
    for (uint32_t c = 0; c < m->cols; ++c) {
      m->data[r][c] = 0.0;
    }
    m->data[r][r] = 1.0;
  }
}
//////////////////////////////////////////////////////////////////////////

void MatrixSetIdentity(matrix_t *m) {
  assert(m);
  assert(m->rows == m->cols);
  MatrixSetIdentityDiag(m);
}
//////////////////////////////////////////////////////////////////////////

void MatrixCopy(matrix_t *src,
                matrix_t *dst) {
  assert(src);
  assert(dst);
  assert(src->rows == dst->rows);
  assert(src->cols == dst->cols);

  for (uint32_t r = 0; r < src->rows; ++r) {
    for (uint32_t c = 0; c < src->cols; ++c) {
      dst->data[r][c] = src->data[r][c];
    }
  }
}
//////////////////////////////////////////////////////////////////////////

void MatrixPrint(matrix_t *m) {
  assert(m);
  for (uint32_t r = 0; r < m->rows; ++r) {
//    QString str = "";
    for (uint32_t c = 0; c < m->cols; ++c) {
//      str += QString::number(m->data[r][c], 'g', 18) + "   ";
    }
//    qDebug() << str;
  }
//  qDebug() << "****";
}
//////////////////////////////////////////////////////////////////////////

void MatrixAdd(matrix_t *ma,
               matrix_t *mb,
               matrix_t *mc) {
  assert(ma);
  assert(mb);
  assert(mc);
  assert(ma->cols == mb->cols && mb->cols == mc->cols);
  assert(ma->rows == mb->rows && mb->rows == mc->rows);

  for (uint32_t r = 0; r < ma->rows; ++r) {
    for (uint32_t c = 0; c < ma->cols; ++c) {
      mc->data[r][c] = ma->data[r][c] + mb->data[r][c];
    }
  }
}
//////////////////////////////////////////////////////////////////////////

void MatrixSubtract(matrix_t *ma,
                    matrix_t *mb,
                    matrix_t *mc) {
  assert(ma);
  assert(mb);
  assert(mc);
  assert(ma->cols == mb->cols && mb->cols == mc->cols);
  assert(ma->rows == mb->rows && mb->rows == mc->rows);

  for (uint32_t r = 0; r < ma->rows; ++r) {
    for (uint32_t c = 0; c < ma->cols; ++c) {
      mc->data[r][c] = ma->data[r][c] - mb->data[r][c];
    }
  }
}
//////////////////////////////////////////////////////////////////////////

void MatrixSubtractFromIdentity(matrix_t *m) {
  assert(m);  
  for (uint32_t r = 0; r < m->rows; ++r) {
    for (uint32_t c = 0; c < m->cols; ++c)
      m->data[r][c] = -m->data[r][c];
    m->data[r][r] = 1.0 + m->data[r][r];
  }
}
//////////////////////////////////////////////////////////////////////////

void MatrixMultiply(matrix_t *ma,
                    matrix_t *mb,
                    matrix_t *mc) {
  assert(ma);
  assert(mb);
  assert(mc);
  assert(ma->cols == mb->rows);
  assert(ma->rows == mc->rows);
  assert(mb->cols == mc->cols);

  for (uint32_t r = 0; r < mc->rows; ++r) {
    for (uint32_t c = 0; c < mc->cols; ++c) {
      mc->data[r][c] = 0.0;
      for (uint32_t rc = 0; rc < ma->cols; ++rc) {
        mc->data[r][c] += ma->data[r][rc]*mb->data[rc][c];
      }
    } //for col
  } //for row
}
//////////////////////////////////////////////////////////////////////////

void MatrixMultiplyByTranspose(matrix_t *ma,
                               matrix_t *mb,
                               matrix_t *mc) {
  assert(ma);
  assert(mb);
  assert(mc);
  assert(ma->cols == mb->cols);
  assert(ma->rows == mc->rows);
  assert(mb->rows == mc->cols);
  for (uint32_t r = 0; r < mc->rows; ++r) {
    for (uint32_t c = 0; c < mc->cols; ++c) {
      mc->data[r][c] = 0.0;
      for (uint32_t rc = 0; rc < ma->cols; ++rc) {
        mc->data[r][c] += ma->data[r][rc] * mb->data[c][rc];
      }
    } //for col
  } //for row
}
//////////////////////////////////////////////////////////////////////////

void MatrixTranspose(matrix_t *mtxin,
                     matrix_t *mtxout) {
  assert(mtxin);
  assert(mtxout);
  assert(mtxin->rows == mtxout->cols);
  assert(mtxin->cols == mtxout->rows);
  for (uint32_t r = 0; r < mtxin->rows; ++r) {
    for (uint32_t c = 0; c < mtxin->cols; ++c) {
      mtxout->data[c][r] = mtxin->data[r][c];
    } //for col
  } //for row
}
//////////////////////////////////////////////////////////////////////////

bool MatrixEq(matrix_t *ma,
              matrix_t *mb,
              double eps) {
  assert(ma);
  assert(mb);
  if (ma->rows != mb->rows || ma->cols != mb->cols)
    return false;

  for (uint32_t r = 0; r < ma->rows; ++r) {
    for (uint32_t c = 0; c < ma->cols; ++c) {
      if (abs(ma->data[r][c] - mb->data[r][c]) <= eps)
        continue;
      return false;
    } //for col
  } //for row
  return true;
}
//////////////////////////////////////////////////////////////////////////

void MatrixScale(matrix_t *m,
                 double scalar) {
  assert(m);
  uint32_t r, c;
  for (r = 0; r < m->rows; ++r) {
    for (c = 0; c < m->cols; ++c) {
      m->data[r][c] *= scalar;
    } //for col
  } //for row
}
//////////////////////////////////////////////////////////////////////////

void swapRows(matrix_t *m,
              uint32_t r1,
              uint32_t r2) {
  assert(m);  
  assert(r1 < m->rows && r2 < m->rows);
  std::swap<double*>(m->data[r1], m->data[r2]);
}
//////////////////////////////////////////////////////////////////////////

void scaleRow(matrix_t *m,
              uint32_t r,
              double scalar) {
  assert(m);
  assert(r < m->rows);
  for (uint32_t c = 0; c < m->cols; ++c)
    m->data[r][c] *= scalar;
}
//////////////////////////////////////////////////////////////////////////

/* Add scalar * row r2 to row r1. */
void shearRow(matrix_t *m,
              uint32_t r1,
              uint32_t r2,
              double scalar) {
  assert(m);
  assert(r1 != r2);
  assert(r1 < m->rows && r2 < m->rows);
  uint32_t c;
  for (c = 0; c < m->cols; ++c)
    m->data[r1][c] += m->data[r2][c] * scalar;
}
//////////////////////////////////////////////////////////////////////////

bool MatrixDestructiveInvert(matrix_t *mtxin,
                             matrix_t *mtxout) {
  assert(mtxin);
  assert(mtxout);
  assert(mtxin->cols == mtxin->rows);
  assert(mtxout->cols == mtxin->cols);
  assert(mtxout->rows == mtxin->rows);
  uint32_t r, ri;
  double scalar;
  MatrixSetIdentity(mtxout);

  for (r = 0; r < mtxin->rows; ++r) {
    if (mtxin->data[r][r] == 0.0) { //we have to swap rows here to make nonzero diagonal
      for (ri = r; ri < mtxin->rows; ++ri) {
        if (mtxin->data[ri][ri] != 0.0)
          break;
      }

      if (ri == mtxin->rows)
        return false;  //can't get inverse matrix

      swapRows(mtxin, r, ri);
      swapRows(mtxout, r, ri);
    } //if mtxin->data[r][r] == 0.0

    scalar = 1.0 / mtxin->data[r][r];
    scaleRow(mtxin, r, scalar);
    scaleRow(mtxout, r, scalar);

    for (ri = 0; ri < r; ++ri) {
      scalar = -mtxin->data[ri][r];
      shearRow(mtxin, ri, r, scalar);
      shearRow(mtxout, ri, r, scalar);
    }

    for (ri = r + 1; ri < mtxin->rows; ++ri) {
      scalar = -mtxin->data[ri][r];
      shearRow(mtxin, ri, r, scalar);
      shearRow(mtxout, ri, r, scalar);
    }
  } //for r < mtxin->rows
  return true;
}
//////////////////////////////////////////////////////////////////////////

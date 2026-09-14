_pkgbase=nec_kbd_backlight
pkgname=nec_kbd_backlight-dkms
pkgver=1
pkgrel=1
pkgdesc="NEC LAVIE N15 keyboard backlight driver"
arch=('x86_64')
url="https://www.example.org/"
license=('CC0-1.0')
depends=('dkms')
conflicts=("${_pkgbase}")
# install=${pkgname}.install
source=(
	# "${url}/files/tarball.tar.gz"
	'dkms.conf'
	'nec_kbd_backlight.c'
	'Makefile'
)
md5sums=(
	# use 'updpkgsums'
	'SKIP'
	'SKIP'
	'SKIP'
)

# prepare() {
#   cd ${_pkgbase}-${pkgver}

#   # Patch
#   patch -p1 -i "${srcdir}"/linux-3.14.patch
# }

package() {
  # Copy dkms.conf
  local dest="${pkgdir}"/usr/src/${_pkgbase}-${pkgver}
  install -Dm644 dkms.conf "${dest}"/dkms.conf
  install -Dm644 Makefile "${dest}"/Makefile
  install -Dm644 nec_kbd_backlight.c "${dest}"/nec_kbd_backlight.c

  # Set name and version
  sed -e "s/@_PKGBASE@/${_pkgbase}/" \
      -e "s/@PKGVER@/${pkgver}/" \
      -i "${dest}"/dkms.conf
}

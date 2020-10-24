/*
 * This file is provided under a MIT license.  When using or
 * redistributing this file, you may do so under either license.
 *
 * MIT License
 *
 * Copyright (c) 2020 Pavel Nadein
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 * "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
 * LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
 * A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
 * OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
 * SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
 * LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
 * DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
 * THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *
 * esp32 open source driver
 *
 * Contact Information:
 * Pavel Nadein <pavelnadein@gmail.com>
 */

#include <esp32_spi.h>

esp_err_t spi_bus_init(spi_host_device_t dev, int msck, int mosi, int miso)
{
	spi_bus_config_t buscfg = {
		.sclk_io_num = msck,
		.mosi_io_num = mosi,
		.miso_io_num = miso,
	};

	return spi_bus_initialize(dev, &buscfg, 0);
}

esp_err_t spi_init(spi_host_device_t dev, spi_device_handle_t *handle,
	int freq, uint8_t mode, uint8_t addr_w)
{
	spi_device_interface_config_t devcfg = {
		.address_bits = addr_w ? addr_w : 1,
		.queue_size = 2,
		.clock_speed_hz = freq,
	};

	return spi_bus_add_device(dev, &devcfg, handle);
}

esp_err_t spi_deinit(spi_device_handle_t *handle)
{
	return spi_bus_remove_device(*handle);
}

esp_err_t spi_write_reg(spi_device_handle_t *handle, gpio_num_t cs,
	uint8_t reg, uint8_t *data, uint16_t size)
{
	spi_transaction_t t = {
		.flags = 0,
		.addr = reg,
		.length = size * 8,
		.tx_buffer = data,
	};
	esp_err_t ret;

	ret = gpio_set_level(cs, 0);
	if (ret)
		return ret;

	ret = spi_device_transmit(*handle, &t);

	gpio_set_level(cs, 1);

	return ret;
}

esp_err_t spi_read_reg(spi_device_handle_t *handle, gpio_num_t cs,
	uint8_t reg, uint8_t *data, uint16_t size)
{
	spi_transaction_t t = {
		.flags = 0,
		.addr = reg,
		.length = size * 8,
		.rx_buffer = data,
	};
	esp_err_t ret;

	ret = gpio_set_level(cs, 0);
	if (ret)
		return ret;

	ret = spi_device_transmit(*handle, &t);

	gpio_set_level(cs, 1);

	return ret;
}

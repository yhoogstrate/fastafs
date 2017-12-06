#include <iostream>
#include <fstream>

#include "config.hpp"

#include "twobit_seq.hpp" // #include "twobit_byte.hpp"



twobit_seq::twobit_seq(void) : twobit_data(nullptr), previous_was_N(false), n(0), N(0)
{
}

/**
 */
#if DEBUG
twobit_seq::~twobit_seq(void)
{
    if(this->twobit_data != nullptr) {
        //throw std::runtime_error("2bit/fasta sequence was opened for insertion but not flushed");
        exit(1);
    }
}
#endif //DEBUG


void twobit_seq::add_N()
{
    if(!this->previous_was_N) {
        this->n_starts.push_back(this->n);
    }

    this->previous_was_N = true;
    this->n++;
    this->N++;
}



void twobit_seq::add_nucleotide(unsigned char nucleotide)
{
    switch( (this->n - this->N) % 4) {
    case 0:
        delete this->twobit_data;
        this->twobit_data = new twobit_byte();
        this->twobit_data->set(6, nucleotide);
        break;
    case 1:
        this->twobit_data->set(4, nucleotide);
        break;
    case 2:
        this->twobit_data->set(2, nucleotide);
        break;
    case 3:
        this->twobit_data->set(0, nucleotide);
        this->data.push_back(this->twobit_data->data);
        delete this->twobit_data;
        this->twobit_data = new twobit_byte();
        break;
#if DEBUG
    default:
        throw std::invalid_argument("add_nucleotide needs to be 1, 2 or 3\n");
        break;
#endif //DEBUG
    }

    if(this->previous_was_N) {
        this->n_ends.push_back(this->n - 1);
    }

    this->previous_was_N = false;
    this->n++;
}


/**
 * @brief MAY ONLY BE USED TO INSERT FULL TWOBIT BYTES REPRESENTING FOUR CHARS!
 *  otherwise use add_twobit_sticky_end(twobit_byte& tb, i < 4);
 */
void twobit_seq::add_twobit(twobit_byte &tb)
{
    this->data.push_back(tb.data);
    this->n++;
}


void twobit_seq::close_reading()
{
    unsigned char sticky_end = (this->n - this->N) % 4;
    if(sticky_end != 0) { // sticky end
        signed int i;
        // if i = 1, last three 2bits need to be set to 00
        // 3 - i = 2,
        for(i = 3 - (signed int) sticky_end; i >= 0; i--) {
            this->twobit_data->set((unsigned char) (i * 2), 0);
        }

        // insert sticky-ended twobit
        this->data.push_back(this->twobit_data->data);
    }

    if(this->previous_was_N) {
        this->n_ends.push_back(this->n - 1);
    }

    delete this->twobit_data;
    this->twobit_data = nullptr;
}






void twobit_seq::print(void)
{
#if DEBUG
    if(this->n_starts.size() != this->n_ends.size()) {
        throw std::invalid_argument("unequal number of start and end positions for N regions\n");
    }
#endif //DEBUG

    bool in_N = false;
    twobit_byte t = twobit_byte();
    unsigned int i;

    printf(">%s (size=%i [ACTG: %i, N: %i], compressed ACTG=%i)\n", this->name.c_str(), this->n, this->n - this->N, this->N, (unsigned int) this->size());

    printf("\nN[s]: ");
    for(i = 0; i < this->n_starts.size(); i++) {
        printf("%i\t", this->n_starts[i]);
    }
    printf("\nN[e]: ");
    for(i = 0; i < this->n_ends.size(); i++) {
        printf("%i\t", this->n_ends[i]);
    }
    printf("\n----------------------\n");

    unsigned int i_n_start = 0;//@todo make iterator
    unsigned int i_n_end = 0;//@todo make iterator
    unsigned int i_in_seq = 0;
    unsigned int chunk_offset;
    const char *chunk;

    for(i = 0; i < this->n; i++) {
        if(i % 4 == 0 and i != 0) {
            printf("\n");
        }
        printf("%i\t", i);



        if(this->n_starts.size() > i_n_start and i == this->n_starts[i_n_start]) {
            in_N = true;
        }

        if(in_N) {
            printf("N\n");

            if(i == this->n_ends[i_n_end]) {
                i_n_end++;
                in_N = false;
            }
        } else {
            // load new twobit chunk when needed
            chunk_offset = i_in_seq % 4;
            if(chunk_offset == 0) {
                t.data = this->data[i_in_seq / 4];
                chunk = t.get();
            }
            printf("%c\n", chunk[chunk_offset]);

            i_in_seq++;
        }
    }
    printf("\n\n");
}

size_t twobit_seq::size(void)
{
    return this->data.size();
}


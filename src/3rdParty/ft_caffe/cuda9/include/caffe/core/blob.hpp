#ifndef __FACETHINK_CAFFE_CORE_BLOB_HPP__
#define __FACETHINK_CAFFE_CORE_BLOB_HPP__

#include "caffe/core/common.hpp"
#include "caffe/core/syncedmemory.hpp"

namespace facethink {

  template <typename Dtype>
  class EXPORT_CLASS Blob{
  public:
    Blob():data_(), count_(0), capacity_(0) {}
    Blob(const std::vector<int>& shape);

    void Reshape(const std::vector<int>& shape);
    void ReshapeLike(const Blob& blob);

    inline std::shared_ptr<SyncedMemory> data() { return data_; }
    void ShareData(const Blob& other);
    
    const Dtype* cpu_data() const;
    Dtype* mutable_cpu_data();

#ifndef CPU_ONLY
    const Dtype* gpu_data() const;
    Dtype* mutable_gpu_data();
#endif

    inline const std::vector<int>& shape() const { return shape_; }
    inline const std::shared_ptr<SyncedMemory>& data() const { return data_; }

    inline int num_axes() const { return shape_.size(); }
    inline int count() const { return count_; }


    /**
     * @brief Compute the volume of a slice; i.e., the product of dimensions
     *        among a range of axes.
     *
     * @param start_axis The first axis to include in the slice.
     *
     * @param end_axis The first axis to exclude from the slice.
     */
    inline int count(int start_axis, int end_axis) const {
      if (start_axis > end_axis || start_axis < 0 ||
	  end_axis < 0 || start_axis > num_axes() ||
	  end_axis > num_axes()) {
	BOOST_LOG_TRIVIAL(error)<<"Blob: invalid index to compute count.";
      }
      int count = 1;
      for (int i = start_axis; i < end_axis; ++i) {
	count *= shape_[i];
      }
      return count;
    }

    inline int count(int start_axis) const {
      return count(start_axis, num_axes());
    }

    inline int shape(int index) const {
      return shape_[CanonicalAxisIndex(index)];
    }

    inline int CanonicalAxisIndex(int axis_idx) const {
      if (abs(axis_idx) >= num_axes()) {
	BOOST_LOG_TRIVIAL(error)<<"Blob: invalid axis index to compute CanonicalAxisIndex.";
      }

      if (axis_idx < 0) {
	return axis_idx + num_axes();
      }
      return axis_idx;
    }

	/// @brief Deprecated legacy shape accessor num: use shape(0) instead.
	inline int num() const { return LegacyShape(0); }
	/// @brief Deprecated legacy shape accessor channels: use shape(1) instead.
	inline int channels() const { return LegacyShape(1); }
	/// @brief Deprecated legacy shape accessor height: use shape(2) instead.
	inline int height() const { return LegacyShape(2); }
	/// @brief Deprecated legacy shape accessor width: use shape(3) instead.
	inline int width() const { return LegacyShape(3); }
	inline int LegacyShape(int index) const {
		if ( num_axes() > 4 ) {
			BOOST_LOG_TRIVIAL(error) << "Cannot use legacy accessors on Blobs with > 4 axes.";
		}

		if ( index >= 4 || index < -4) {
			BOOST_LOG_TRIVIAL(error) << "shape index of Blobs >= 4 of < -4.";
		}

		if (index >= num_axes() || index < -num_axes()) {
			// Axis is out of range, but still in [0, 3] (or [-4, -1] for reverse
			// indexing) -- this special case simulates the one-padding used to fill
			// extraneous axes of legacy blobs.
			return 1;
		}
		return shape(index);
	}

    inline int offset(const int n, const int c = 0, const int h = 0,
		      const int w = 0) const {
      return ((n * shape(1) + c) * shape(2) + h) * shape(3) + w;
    }

    inline std::string shape_string() const {
      std::ostringstream stream;
      for (size_t i = 0; i < shape_.size(); ++i) {
	stream << shape_[i] << " ";
      }
      stream << "(" << count_ << ")";
      return stream.str();
    }

    // import data
    void ImportFromExtenalData(const Dtype* data_ext, int num);
    void ImportFrom(const std::vector<Dtype>& v);

  private:
    inline bool CheckData() const {
      if (data_ == nullptr) {
	BOOST_LOG_TRIVIAL(error)<<"Blob: Invalid data nullptr";
	return false;
      }
      return true;
    }
    
  private:
    std::vector<int> shape_;
    std::shared_ptr<SyncedMemory> data_;
    int count_;
    int capacity_;
    
    DISABLE_COPY_AND_ASSIGN(Blob);
  };// class Blob

}// namespace facethink


#endif
